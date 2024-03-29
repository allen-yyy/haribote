/* マルチタスク関係 */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;
struct pid_t pid;
int taskruns=0,taskfree=0; 
struct OFIFO32 free_ofifo;

struct TASK *pid2task(int pids)
{
	return pid.pido[pids].task;
}

void init_pid(struct MEMMAN *memman)
{
	int i;

	pid.next = -1;
	for(i=0;i<= MAX_PID;++i)
	{
		pid.pido[i].pid = i;
	}
	return;
}

int pid_alloc(struct TASK *task)
{
	io_cli();
	pid.next++;
	pid.pido[pid.next].task = task;
	task->pid = pid.next;
	io_sti();
	return pid.next;
}

struct TASK *task_now(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

void task_add(struct TASK *task)
{
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2; /* 動作中 */
	return;
}

void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	/* taskがどこにいるかを探す */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* ここにいた */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* ずれるので、これもあわせておく */
	}
	if (tl->now >= tl->running) {
		/* nowがおかしな値になっていたら、修正する */
		tl->now = 0;
	}
	task->flags = 1; /* スリープ中 */
	
	/* ずらし */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
	return;
}

void task_delete(struct TASK *task)
{
	task_remove(task);
	memset(task,0,sizeof(struct TASK));
	return;
} 

void task_switchsub(void)
{
	int i;
	/* 一番上のレベルを探す */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* 見つかった */
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

void task_idle(void)
{
	for (;;) {
		task_now()->name="cpu idle";
		io_hlt();
	}
}

struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	
	init_pid(memman);

	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		taskctl->tasks0[i].tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
		set_segmdesc(gdt + TASK_GDT0 + MAX_TASKS + i, 15, (int) taskctl->tasks0[i].ldt, AR_LDT);
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}

	task = task_alloc();
	task->name="haribote kernel";
	task->devflag=0;
	task->flags = 2;	/* 動作中マーク */
	task->priority = 2; /* 0.02秒 */
	task->level = 0;	/* 最高レベル */
	task_add(task);
	task_switchsub();	/* レベル設定 */
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);
	
	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	idle->devflag=0;
	task_run(idle, MAX_TASKLEVELS - 1, 1); 
	taskctl->task_fpu = 0; 
	
	free_ofifo.buf=(struct OFIFO32 *) memman_alloc_4k(memman, 1000);
	ofifo32_init(&free_ofifo,1000,free_ofifo.buf,NULL);

	return task;
}

struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 使用中マーク */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* とりあえず0にしておくことにする */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.iomap = 0x40000000;
			task->tss.ss0 = 0;
			task->fpu[0] = 0x037f; /* CW(control word) */  /* ･ｳ･ｳ､ｫ､ */
            task->fpu[1] = 0x0000; /* SW(status word)  */
            task->fpu[2] = 0xffff; /* TW(tag word)     */
            for (i = 3; i < 108 / 4; i++) {
                task->fpu[i] = 0;
            }  
			task->pid = pid_alloc(task); 
			task->message_r = (struct MESSAGE *)memman_alloc_4k(memman, sizeof(struct MESSAGE));                                          /* ･ｳ･ｳ､ﾞ､ﾇ */
			task->devflag=0;
			return task;
		}
	}
	return 0; /* もう全部使用中 */
}

void task_run(struct TASK *task, int level, int priority)
{
	if (level < 0) {
		level = task->level; /* レベルを変更しない */
	}
	if (priority > 0) {
		task->priority = priority;
	}
	if (task->flags == 2 && task->level != level) { /* 動作中のレベルの変更 */
		task_remove(task); /* これを実行するとflagsは1になるので下のifも実行される */
	}
	if (task->flags != 2||task->flags == 3) {
		/* スリープから起こされる場合 */
		task->level = level;
		task_add(task);
	}

	taskctl->lv_change = 1; /* 次回タスクスイッチのときにレベルを見直す */
	return;
}

void task_sleep(struct TASK *task)
{
	struct TASK *now_task;
	if (task->flags == 2) {
		/* 動作中だったら */
		now_task = task_now();
		task_remove(task); /* これを実行するとflagsは1になる */
		if (task == now_task) {
			/* 自分自身のスリープだったので、タスクスイッチが必要 */
			task_switchsub();
			now_task = task_now(); /* 設定後での、「現在のタスク」を教えてもらう */
			if(now_task->devflag) asm("movl %%esp,%0":"=r"(now_task->tss.esp0)); 
			farjmp(0, now_task->sel);
		}
	}
	return;
}

void task_switch(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	L2:
	tl->now++;
	if (tl->now >= tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	if(new_task->flags==0) goto L2;
	if(taskruns==100)
	{
		taskruns=0;
		//printk("      %d%%",taskfree);
		ofifo32_put(&free_ofifo,taskfree);
		taskfree=0;
	}
	if(new_task->pid==10001) taskfree++;
	//printk("%d",new_task->pid);
	taskruns++;
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task && new_task->sel != now_task->sel) {
		if(new_task->devflag) asm("movl %%esp,%0":"=r"(new_task->tss.esp0)); 
		farjmp(0, new_task->sel);
	}
	return;
}

int *inthandler07(int *esp)
{
    struct TASK *now = task_now();
    io_cli();
    clts();
    if (taskctl->task_fpu != now) {
        if (taskctl->task_fpu != 0) {
            fnsave(taskctl->task_fpu->fpu);
        }
        frstor(now->fpu);
        taskctl->task_fpu = now;
    }
    io_sti();
    return 0;
}

void task_block(struct TASK *task)
{
	struct TASK *now_task;
	now_task = task_now();
	task_remove(task);
	task->flags = 3;
	if (task == now_task) {
		task_switchsub();
		now_task = task_now();
		if(now_task->devflag) asm("movl %%esp,%0":"=r"(now_task->tss.esp0)); 
		farjmp(0, now_task->sel);
	}
	return;
}

void task_unblock(struct TASK *task)
{
	task_run(task,task->level,task->priority);
	return;
}

int message_receive(int to_receive,struct MESSAGE *message)
{
	struct TASK *task = task_now();
	for(;;)
	{
		io_cli();
		task->r_flags = 1;
		if(task->s_flag == 1)
		{
			if(to_receive==ANY)
			{
				task->r_flags = 0;
				task->s_flag = 0;
				memcpy(message,task->message_r,sizeof(struct MESSAGE));
				io_sti();
				return 0;
			}else if(to_receive==task->message_r->src)
			{
				task->r_flags = 0;
				task->s_flag = 0;
				memcpy(task->message_r,message,sizeof(struct MESSAGE));
				io_sti();
				return 0;
			}else{
				task->s_flag = 0;
				goto no_message;
			}
		}
		no_message:
			io_sti();
			task_block(task);
	} 
	return -1;
}

int message_send(int to_send,struct MESSAGE *message)
{
	struct TASK *task = pid2task(to_send);
	if(task==NULL)
	{
		return -1;
	}
	io_cli();
	message->src = task2pid(task_now());
	memcpy(task->message_r,message,sizeof(struct MESSAGE));
	task->s_flag = 1;
	io_sti();
	if(task->r_flags) task_unblock(task);
	return 0;
}
