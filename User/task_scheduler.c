#include "task_scheduler.h"
#include <stdlib.h>

TaskScheduler gTaskScheduler;

#ifndef SYSTICK_INTERRUPT_FREQ
    #define SYSTICK_INTERRUPT_FREQ (1000u)
#endif 

#ifndef MAIN_LOOP_TASK_CLK_DIV
    #define MAIN_LOOP_TASK_CLK_DIV (1u)
#endif



int32_t TaskSchedulerInit() {
    gTaskScheduler.count = 0;
    gTaskScheduler.interrupt_count = 0;
    gTaskScheduler.is_idle = 1;
    gTaskScheduler.time_interval = 1000000 / SYSTICK_INTERRUPT_FREQ;
    gTaskScheduler.time_interval_count = MAIN_LOOP_TASK_CLK_DIV;
    gTaskScheduler.idle_task_list = NULL;
    gTaskScheduler.interrupt_task_list = NULL;
    gTaskScheduler.main_loop_task_list = NULL;
    gTaskScheduler.reset_callback = NULL;
    gTaskScheduler.reset_count = -1;
    return 0;
}

uint32_t GetSystemTick(void) {
    return gTaskScheduler.count;
}

void TaskSchedulerInterrupt(void) {
    gTaskScheduler.interrupt_count++;

    Task * task = gTaskScheduler.interrupt_task_list;
    while(1) {
        if(task == NULL) {
            break;
        }
        if(task->phase_diff == gTaskScheduler.interrupt_count % task->period) {
            task->task_entry(task->input_obj);
        }
        task = task->next;
    }

    if(gTaskScheduler.interrupt_count % gTaskScheduler.time_interval_count == 0) {
        gTaskScheduler.is_idle = 0;
        gTaskScheduler.count++;
    }

    if(gTaskScheduler.reset_count > 0) {
        gTaskScheduler.reset_count--;
    } else if(gTaskScheduler.reset_count == 0) {
        if(gTaskScheduler.reset_callback != NULL) {
            gTaskScheduler.reset_callback();
        }
    }
}

void TaskSchedulerMainLoop(void) {
    if(!gTaskScheduler.is_idle) {
        Task * task = gTaskScheduler.main_loop_task_list;
        while (task != NULL) {
            if(task->phase_diff == gTaskScheduler.count % task->period) {
                task->task_entry(task->input_obj);
            }
            task = task->next;
        }
        gTaskScheduler.is_idle = 1;
    }

    Task * task = gTaskScheduler.idle_task_list;
    while (task != NULL) {
        task->task_entry(task->input_obj);
        task = task->next;
    }
}



int32_t TaskCreate(
    Task * task, 
    TaskType type,
    uint32_t period, 
    uint32_t phase_diff, 
    uint32_t priority, 
    void * (*task_entry)(void *), 
    void * input_obj
) {
    if(task == NULL || task_entry == NULL) {
        return -1;
    }
    Task * task_list = NULL;
    Task ** task_list_ptr = NULL;

    phase_diff %= period;

    task->input_obj = input_obj;
    task->task_entry = task_entry;

    task->priority = priority;
    task->type = type;

    task->next = NULL;

    if(period != 0) {
        uint32_t time_interval = 0;
        switch(task->type) {
            case InterruptTask:
                time_interval = gTaskScheduler.time_interval;
                task_list = gTaskScheduler.interrupt_task_list;
                task_list_ptr = &gTaskScheduler.interrupt_task_list;
                break;
            case MainLoopTask:
                time_interval = gTaskScheduler.time_interval * gTaskScheduler.time_interval_count;
                task_list = gTaskScheduler.main_loop_task_list;
                task_list_ptr = &gTaskScheduler.main_loop_task_list;
                break;
        }
        task->period = period / time_interval;
        task->phase_diff = phase_diff / time_interval;
        if(task->period == 0) {
            task->period = 1;
            task->phase_diff = 0;
        }
    } else {
        task_list = gTaskScheduler.idle_task_list;
        task_list_ptr = &gTaskScheduler.idle_task_list;
    }
    while (1) {
        if(task_list == NULL) {
            *task_list_ptr = task;
            break;
        }
        if(task->priority < task_list->priority) {
            *task_list_ptr = task;
            task->next = task_list;
            break;
        }
        task_list_ptr = &task_list->next;
        task_list = task_list->next;
    }
    return 0;
}

