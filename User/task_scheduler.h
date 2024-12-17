#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdint.h>
	 
	 
typedef enum _TaskType {
	MainLoopTask, InterruptTask
} TaskType;

typedef struct _Task Task;

typedef struct _Task {
	Task * next;
	TaskType type;					
	uint32_t priority;					//smaller value has higher priority
	int32_t period;						//us in interrupt task, ms in main loop task
	int32_t phase_diff;					//us
	void * (*task_entry)(void *);
	void * input_obj;
} Task;
	 
typedef struct _TaskScheduler {
	Task * main_loop_task_list;
	Task * interrupt_task_list;
	Task * idle_task_list;
	void (*reset_callback)(void);
	int32_t reset_count;
	uint32_t count;						//ms
	uint32_t interrupt_count;
	uint32_t time_interval;				//us
	uint32_t time_interval_count;
	int8_t is_idle;
} TaskScheduler;


void TaskSchedulerInterrupt(void);

void TaskSchedulerMainLoop(void);

int32_t TaskCreate(
	Task * task, 
	TaskType type, 
	uint32_t period, 
	uint32_t phase_diff, 
	uint32_t priority,  // the low value has a high priority
	void * (*task_entry)(void *), 
	void * input_obj
);


extern TaskScheduler gTaskScheduler;

extern int32_t TaskSchedulerInit(void);

extern uint32_t GetSystemTick(void);

	 
#ifdef __cplusplus
}
#endif

#endif /* __TASK_SCHEDULER_H__ */
