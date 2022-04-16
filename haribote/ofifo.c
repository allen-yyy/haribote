/* FIFOライブラリ */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void ofifo32_init(struct OFIFO32 *fifo, int size, int *buf, struct TASK *task)
/* FIFOバッファの初期化 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->flags = 0;
	fifo->p = 0; /* 書き込み位置 */
	fifo->q = 0; /* 読み込み位置 */
	fifo->task = task; /* データが入ったときに起こすタスク */
	return;
}

int ofifo32_put(struct OFIFO32 *fifo, int data)
/* OFIFOへデータを送り込んで蓄える */
{
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* タスクが寝ていたら */
			task_run(fifo->task, -1, 0); /* 起こしてあげる */
		}
	}
	return 0;
}

int ofifo32_get(struct OFIFO32 *fifo)
/* OFIFOからデータを一つとってくる */
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
