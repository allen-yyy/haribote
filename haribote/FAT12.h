void FAT12_readfat(int *fat, unsigned char *img);
void FAT12_loadfile(int clustno, int size, char *buf, int *fat, char *img); 
struct FILEINFO *FAT12_search(char *name, struct FILEINFO *finfo, int max);
