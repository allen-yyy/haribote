/* FIFO���C�u���� */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void ofifo32_init(struct OFIFO32 *fifo, int size, int *buf, struct TASK *task)
/* FIFO�o�b�t�@�̏����� */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->flags = 0;
	fifo->p = 0; /* �������݈ʒu */
	fifo->q = 0; /* �ǂݍ��݈ʒu */
	fifo->task = task; /* �f�[�^���������Ƃ��ɋN�����^�X�N */
	return;
}

int ofifo32_put(struct OFIFO32 *fifo, int data)
/* OFIFO�փf�[�^�𑗂荞��Œ~���� */
{
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* �^�X�N���Q�Ă����� */
			task_run(fifo->task, -1, 0); /* �N�����Ă����� */
		}
	}
	return 0;
}

int ofifo32_get(struct OFIFO32 *fifo)
/* OFIFO����f�[�^����Ƃ��Ă��� */
{
	int data;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	return data;
}

int ofifo32_status(struct OFIFO32 *fifo)
{
	return FLAGS_OVERRUN;	/*cann't know the size*/
}
