#include "task.h"
#include "led_task.h"
#include "log_task.h"
#include "uart_task.h"
#include "stimulation_task.h"
#include "ads1299_task.h"
#include "dip_task.h"
#include "bluetooth_task.h"
#include "getstimulation_task.h"
#include "task_scheduler.h"
#include <stdlib.h>


Task mLogTask;
Task mADS1299Task;
Task mUartTask;
Task mDipTask;
Task mCCATask;
Task mBuletooth;
Task mStimulationTask;

void TaskInit(){
	//LogInit();
	ADS1299TaskInit();
	UartTaskInit();
	//CCATaskInit();
	BluetoothInit();
	
	TaskCreate(&mDipTask, MainLoopTask, 0, 0, 0, GetSysModeEntry, NULL);
	TaskCreate(&mADS1299Task, MainLoopTask, 0, 0, 0, ADS1299TaskInterruptEntry, NULL);
	TaskCreate(&mUartTask, MainLoopTask, 50000, 0, 0, UartDataFlushTaskEntry, NULL);
	TaskCreate(&mCCATask, MainLoopTask, 0, 0, 0, FFTTaskEntry, NULL);
	TaskCreate(&mBuletooth, MainLoopTask, 0, 0, 0, BluetoothSendEntry, NULL);
	TaskCreate(&mStimulationTask, MainLoopTask, 10000, 0, 0, StimulationTaskEntry, NULL);
}

