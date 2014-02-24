#ifndef _TASK_H
#define _TASK_H

#include <stdint.h>
#include "vmm.h"

#define KE_USER_START 0x00400000
#define KE_KERNEL_START 0x80000000
#define MAX_THREAD 5
#define PROCESS_STATE_SLEEP 0
#define PROCESS_STATE_ACTIVE 1

typedef struct trapFrame{
	uint32_t esp;
	uint32_t ebp;
	uint32_t eip;
	uint32_t edi;
	uint32_t esi;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t flags;

}trapFrame;

struct _process;

typedef struct _thread{
	struct _process* parent;
	void* initialStack;
	void* stackLimit;
	void* kernelStack;
	uint32_t priority;
	int state;
	trapFrame frame;
	uint32_t imageBase;
	uint32_t imageSize;
}thread;

typedef struct _process{
	int id;
	int priority;
	pdirectory* pageDirectory;
	int state;
	int threadCount;
	struct _thread threads[MAX_THREAD];
}process;


extern int createThread (int (*entry) (void), uint32_t stackBase);
extern int terminateThread (thread* handle);
extern int createProcess (char* appname);
extern void executeProcess ();
extern void TermintateProcess();

#endif
