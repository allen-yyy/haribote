/* ƒ}ƒ‹ƒ`ƒ^ƒXƒNŠÖŒW */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *pid2task(int pid)
{
	struct TASK *task;
	struct pid_t *pids = *((int *) 0x0f0a);
	task = pids->pido[pid].task;
	return task;
}

void init_pid()
{
	int i;
	struct pid_t *pid;
	struct blocks_t *block;
	pid->next = 0;
	block->next = 0;
	for(i=0;i<=MAX_PID;++i)
	{
		pid->pido[i].pid = i;
	}
	for(i=0;i<=MAX_TASKS;++i)
	{
		block->blocko[i].pid = i;
	}
	*((int *) 0x0f0a) = pid;
	*((int *) 0x0f0f) = block;
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
	task->flags = 2; /* “®ì’† */
	return;
}

void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	struct pid_t *pid = *((int *) 0x0f0a);
	
	/* task‚ª‚Ç‚±‚É‚¢‚é‚©‚ğ’T‚· */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* ‚±‚±‚É‚¢‚½ */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* ‚¸‚ê‚é‚Ì‚ÅA‚±‚ê‚à‚ ‚í‚¹‚Ä‚¨‚­ */
	}
	if (tl->now >= tl->running) {
		/* now‚ª‚¨‚©‚µ‚È’l‚É‚È‚Á‚Ä‚¢‚½‚çAC³‚·‚é */
		tl->now = 0;
	}
	task->flags = 1; /* ƒXƒŠ[ƒv’† */
	
	/* ‚¸‚ç‚µ */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
	
	pid->pido[task->pid].task->flags=task->flags; 
	return;
}

void task_switchsub(void)
{
	int i;
	/* ˆê”Ôã‚ÌƒŒƒxƒ‹‚ğ’T‚· */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* Œ©‚Â‚©‚Á‚½ */
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

void task_idle(void)
{
	for (;;) {
		io_hlt();
	}
}

struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

	init_pid();

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
	task->flags = 2;	/* “®ì’†ƒ}[ƒN */
	task->priority = 2; /* 0.02•b */
	task->level = 0;	/* Å‚ƒŒƒxƒ‹ */
	task_add(task);
	task_switchsub();	/* ƒŒƒxƒ‹İ’è */
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
	task_run(idle, MAX_TASKLEVELS - 1, 1);
	
	taskctl->task_fpu = 0; 

	return task;
}

struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* g—p’†ƒ}[ƒN */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* ‚Æ‚è‚ ‚¦‚¸0‚É‚µ‚Ä‚¨‚­‚±‚Æ‚É‚·‚é */
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
			task->fpu[0] = 0x037f; /* CW(control word) */  /* ¥³¥³¤«¤é */
            task->fpu[1] = 0x0000; /* SW(status word)  */
            task->fpu[2] = 0xffff; /* TW(tag word)     */
            for (i = 3; i < 108 / 4; i++) {
                task->fpu[i] = 0;
            }                                              /* ¥³¥³¤Ş¤Ç */
			return task;
		}
	}
	return 0; /* ‚à‚¤‘S•”g—p’† */
}

void task_run(struct TASK *task, int level, int priority)
{
	//int i;
	struct pid_t *pid = *((int *) 0x0f0a);
	if (level < 0) {
		level = task->level; /* ƒŒƒxƒ‹‚ğ•ÏX‚µ‚È‚¢ */
	}
	if (priority > 0) {
		task->priority = priority;
	}
	if(task->flags == 1){
		pid->pido[pid->next].task = task;
		pid->next++;
		task->pid=pid->next-1;
	}
	if (task->flags == 2 && task->level != level) { /* “®ì’†‚ÌƒŒƒxƒ‹‚Ì•ÏX */
		
		task_remove(task); /* ‚±‚ê‚ğÀs‚·‚é‚Æflags‚Í1‚É‚È‚é‚Ì‚Å‰º‚Ìif‚àÀs‚³‚ê‚é */
	}
	if (task->flags != 2||task->flags == 3) {
		pid->pido[pid->next].task = task;
		pid->next++;
		task->pid=pid->next-1;
		/* ƒXƒŠ[ƒv‚©‚ç‹N‚±‚³‚ê‚éê‡ */
		task->level = level;
		task_add(task);
	}

	taskctl->lv_change = 1; /* Ÿ‰ñƒ^ƒXƒNƒXƒCƒbƒ`‚Ì‚Æ‚«‚ÉƒŒƒxƒ‹‚ğŒ©’¼‚· */
	return;
}

void task_sleep(struct TASK *task)
{
	struct TASK *now_task;
	if (task->flags == 2) {
		/* “®ì’†‚¾‚Á‚½‚ç */
		now_task = task_now();
		task_remove(task); /* ‚±‚ê‚ğÀs‚·‚é‚Æflags‚Í1‚É‚È‚é */
		if (task == now_task) {
			/* ©•ª©g‚ÌƒXƒŠ[ƒv‚¾‚Á‚½‚Ì‚ÅAƒ^ƒXƒNƒXƒCƒbƒ`‚ª•K—v */
			task_switchsub();
			now_task = task_now(); /* İ’èŒã‚Å‚ÌAuŒ»İ‚Ìƒ^ƒXƒNv‚ğ‹³‚¦‚Ä‚à‚ç‚¤ */
			farjmp(0, now_task->sel);
		}
	}
	return;
}

void task_switch(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
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
	struct pid_t *pid = *((int *) 0x0f0a);
	struct TASK *now_task;
	struct blocks_t *block = *((int *) 0x0f0f);
	if (task->flags == 2) {
		
		//now_task = task_now();
		//task_remove(task);
		task->flags=3;
		pid->pido[task->pid].task->flags=task->flags;
		block->blocko[block->next].task = task;
		block->blocko[block->next].pid = task->pid;
		block->next++;
		//if (task == now_task) {
		//	
			task_switchsub();
			now_task = task_now();
			farjmp(0, now_task->sel);
		//}
		 
	}
	return;
}

void task_unblock(struct TASK *task)
{
	struct pid_t *pid = *((int *) 0x0f0a);
	struct TASK *now_task;
	struct blocks_t *block = *((int *) 0x0f0f);
	int i; 
	//if (task->flags == 2) {
	
	//now_task = task_now();
	task_run(task,task->level,task->priority);
	//task->flags=1;
	pid->pido[task->pid].task->flags=task->flags;
		//block->blocko[block->next].task = task; 
		//if (task == now_task) {
		//	
		//	task_switchsub();
		//	now_task = task_now();
		//	farjmp(0, now_task->sel);
		//}
	for (i = 0; i < block->next; i++) {
		if (block->blocko[i].task == task) {
			/* ‚±‚±‚É‚¢‚½ */
			block->blocko[i].task = 0;
			block->blocko[i].pid = 0;
			break;
		}
	}
	for (; i < block->next; i++) {
		block->blocko[i] = block->blocko[i + 1];
	}
	block->next--;
	//}
	return;
}

int message_receive(int to_receive,struct MESSAGE *message)
{
	struct TASK *task = task_now();
	//struct TASK *to_task;
	task->r_flags = 1;
	task_block(task);
	for(;;)
	{
		if(task->message_r!=0)
		{
			if(to_receive==ANY)
			{
				task_unblock(task);
				message=task->message_r;
				return 0;
			}else{
				if(task->message_r->src==to_receive)
				{
					task_unblock(task);
					message = task->message_r;
					return 0;
				}
			}
		} 
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
	message->src = task2pid(task_now());
	task->message_r = message;
	return 0;
}
