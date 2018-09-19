/*
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 16:00
 * Email        : lidianjun@yeah.net
 * Filename     : AppFreeRTOSConfig.h
 * Description  : 
 * 
 */
 
 
#ifndef APP_FREERTOS_CONFIG_H
#define APP_FREERTOS_CONFIG_H

/* Task name. */
#define appConfigSYSTEM_TASK_NAME "SYS"
#define appConfigKEY_TASK_NAME    "KEY"
#define appConfigLOG_TASK_NAME    "LOG"

/* Task priorities. */
#define appConfigSYSTEM_TASK_PRI                ( tskIDLE_PRIORITY + 1 )
#define appConfigKEY_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define appConfigLOG_TASK_PRIORITY				( tskIDLE_PRIORITY + 0 )

/* unused Task priorities.*/
#define appConfigCMD_LINE_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define appConfigLCD_PRINT_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* Task stack size. */
#define appConfigSYSTEM_TASK_STACKSIZE          (configMINIMAL_STACK_SIZE * 1)   
#define appConfigKeySTACK_SIZE					(configMINIMAL_STACK_SIZE * 1)
#define appConfigLogSTACK_SIZE					(configMINIMAL_STACK_SIZE * 1)

/* unused Task stack size.*/
#define appConfigcmdlineSTACK_SIZE				(configMINIMAL_STACK_SIZE * 1)
#define appConfigLcdPrintSTACK_SIZE				(configMINIMAL_STACK_SIZE * 1)

#ifndef appConfigSYS_STA_LED_ON
#define appConfigSYS_STA_LED_ON 0
#endif

#ifndef appConfigUART_USE_DMA
#define appConfigUART_USE_DMA 1
#endif

#ifndef appConfigGET_RUN_STA
#define appConfigGET_RUN_STA 1
#endif

#endif

