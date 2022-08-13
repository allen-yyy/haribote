#include "E1000.h"

int io_base,mem_base;
BOOL _has_eeprom = 0;
char *_mac_address;

void write_register(unsigned short offset, unsigned int value);
unsigned int read_register(unsigned short offset);
BOOL detect_eeprom();
unsigned short read_eeprom(unsigned int address);
MacAddress read_mac_address();
void initialize_rx();
void initialize_tx();
void enable_interrupt();
int receive_packet(void *buffer, int size);
int send_packet(const void *buffer, int size);

char *_rx_descriptors_range;
E1000RXDescriptor *_rx_descriptors;
char *_tx_descriptors_range;
E1000TXDescriptor *_tx_descriptors;
char *_rx_buffers[E1000_NUM_RX_DESC];
char *_tx_buffers[E1000_NUM_TX_DESC];
int _current_rx_descriptors = 0;
int _current_tx_descriptors = 0;

void write_register(unsigned short offset, unsigned int value)
{
    if (mem_base)
    {
        (*((volatile unsigned int*)(offset + mem_base)))=(value);
    }
    else
    {
        io_out32(io_base, offset);
        io_out32(io_base + 4, value);
    }
}

unsigned int read_register(unsigned short offset)
{
    if (mem_base)
    {
        return *((volatile unsigned int*)(offset + mem_base));
    }
    else
    {
        io_out32(io_base, offset);
        return io_in32(io_base + 4);
    }
}

BOOL detect_eeprom()
{
    write_register(E1000_REG_EEPROM, 0x1);

    BOOL exists = FALSE;
    int i;

    for (i = 0; i < 1000 && !exists; i++)
    {
        if (read_register(E1000_REG_EEPROM) & 0x10)
        {
            exists = TRUE;
        }
    }

    return exists;
}

unsigned short read_eeprom(unsigned int address)
{
    unsigned short data = 0;
    unsigned int tmp = 0;

    if (_has_eeprom)
    {
        write_register(E1000_REG_EEPROM, (1) | ((unsigned int)(address) << 8));
        while (!((tmp = read_register(E1000_REG_EEPROM)) & (1 << 4)))
        {
        }
    }
    else
    {
        write_register(E1000_REG_EEPROM, (1) | ((unsigned int)(address) << 2));
        while (!((tmp = read_register(E1000_REG_EEPROM)) & (1 << 1)))
        {
        }
    }

    data = (unsigned short)((tmp >> 16) & 0xFFFF);
    return data;
}

char *read_mac_address()
{
    char *address[6];

    if (_has_eeprom)
    {
        unsigned int temp;
        temp = read_eeprom(0);
        address[0] = temp & 0xff;
        address[1] = temp >> 8;
        temp = read_eeprom(1);
        address[2] = temp & 0xff;
        address[3] = temp >> 8;
        temp = read_eeprom(2);
        address[4] = temp & 0xff;
        address[5] = temp >> 8;
    }
    else
    {
        unsigned int mac_low = read_register(E1000_REG_MAC_LOW);
        unsigned int mac_hight = read_register(E1000_REG_MAC_HIGHT);

        address[0] = mac_low & 0xff;
        address[1] = mac_low >> 8 & 0xff;
        address[2] = mac_low >> 16 & 0xff;
        address[3] = mac_low >> 24 & 0xff;

        address[4] = mac_hight & 0xff;
        address[5] = mac_hight >> 8 & 0xff;
    }

    return address;
}

void initialize_rx()
{
    _rx_descriptors_range = kalloc(sizeof(E1000RXDescriptor) * E1000_NUM_RX_DESC);
    _rx_descriptors = (E1000RXDescriptor *) _rx_descriptors_range;

	int i;
    for (i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        _rx_buffers[i]=(char *)kalloc(8192);
        _rx_descriptors[i].address = &_rx_buffers[i][0];
        _rx_descriptors[i].status = 0x1;
    }

    write_register(E1000_REG_RX_LOW, (int) _rx_descriptors_range);
    write_register(E1000_REG_RX_HIGH, 0);
    write_register(E1000_REG_RX_LENGTH, E1000_NUM_RX_DESC * sizeof(E1000RXDescriptor));

    write_register(E1000_REG_RX_HEAD, 0);
    write_register(E1000_REG_RX_TAIL, E1000_NUM_RX_DESC - 1);
    write_register(E1000_REG_RX_CONTROL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}

void initialize_tx()
{
    _tx_descriptors_range = kalloc(sizeof(E1000TXDescriptor) * E1000_NUM_TX_DESC);
    _tx_descriptors = reinterpret_cast<E1000TXDescriptor *>(_tx_descriptors_range->base());

    for (int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        _tx_buffers[i]=(char *)kalloc(8192);
        _tx_descriptors[i].address = &_tx_buffers[i][0];
        _tx_descriptors[i].status = 0xff;
    }

    write_register(E1000_REG_TX_LOW, _tx_descriptors_range->physical_base());
    write_register(E1000_REG_TX_HIGH, 0);
    write_register(E1000_REG_TX_LENGTH, E1000_NUM_TX_DESC * sizeof(E1000TXDescriptor));

    write_register(E1000_REG_TX_HEAD, 0);
    write_register(E1000_REG_TX_TAIL, E1000_NUM_TX_DESC - 1);
    write_register(E1000_REG_TX_CONTROL, TCTL_EN | TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC);
}

void enable_interrupt()
{
    write_register(E1000_REG_IMASK, 0x1F6DC);
    write_register(E1000_REG_IMASK, 0xff & ~4);
    read_register(0xc0);
}

int receive_packet(void *buffer, int size)
{
    unsigned int packet_size = _rx_descriptors[_current_rx_descriptors].length;
    memcpy(buffer,_rx_buffers[_current_rx_descriptors],packet_size);
    _rx_descriptors[_current_rx_descriptors].status=0;

    unsigned short old_cur = _current_rx_descriptors;
    _current_rx_descriptors = (_current_rx_descriptors + 1) % E1000_NUM_RX_DESC;
    write_register(E1000_REG_RX_TAIL, old_cur);

    return packet_size;
}

int send_packet(const void *buffer, int size)
{
    memcpy(_tx_buffers[_current_tx_descriptors],buffer,size);
    _tx_descriptors[_current_tx_descriptors].length = size;
    _tx_descriptors[_current_tx_descriptors].command = CMD_EOP | CMD_IFCS | CMD_RS;
    _tx_descriptors[_current_tx_descriptors].status = 0;

    _current_tx_descriptors = (_current_tx_descriptors + 1) % E1000_NUM_TX_DESC;
    write_register(E1000_REG_TX_TAIL, _current_tx_descriptors);

    return size;
}

struct pci_device_id e1000id[]=
{
	{0x8086,0x100E,0xFFFF,0xFFFF,0,0},
	{0x8086,0x153A,0xFFFF,0xFFFF,0,0},
	{0x8086,0x10EA,0xFFFF,0xFFFF,0,0},
	{0x8086,0x100F,0xFFFF,0xFFFF,0,0},
}

int E1000_init()
{
	if(!pci_scan(e1000id)) return 0;
    struct pci_bar bar0 = pci_get_bar(0);

    if (bar0.type() == PCIBAR_MMIO)
    {
        _mmio_range = make<MMIORange>(bar0.range());
    }
    else
    {
        _pio_base = bar0.base();
    }

    _has_eeprom = detect_eeprom();
    _mac_address = read_mac_address();

    initialize_rx();
    initialize_tx();
    enable_interrupt();
    return 1;
}

driver_init(E1000_init);

void acknowledge_interrupt()
{
    write_register(E1000_REG_IMASK, 0x0);
}

void handle_interrupt()
{
    unsigned int status = read_register(E1000_REG_STATUS);


    if (status & 4)
    {
        unsigned int flags = read_register(E1000_REG_CONTROL);
        write_register(E1000_REG_CONTROL, flags | E1000_CTL_START_LINK);
    }
}

BOOL can_write()
{
    return (_tx_descriptors[_current_tx_descriptors].status & 1);
}

BOOL can_read()
{
    return _rx_descriptors[_current_rx_descriptors].status & 0x1;
}

int read(size64_t offset, void *buffer, int size)
{
    UNUSED(offset);

    int packet_size = receive_packet(buffer, size);

    return packet_size;
}

int write(size64_t offset, const void *buffer, int size)
{
    UNUSED(offset);

    int packet_size = send_packet(buffer, size);

    return packet_size;
}

HjResult call(IOCall request, void *args)
{
    if (request == IOCALL_NETWORK_GET_STATE)
    {
        IOCallNetworkSateAgs *state = (IOCallNetworkSateAgs *)args;
        state->mac_address = _mac_address;
        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}
