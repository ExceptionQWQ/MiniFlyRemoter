/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "adc.h"
#include "stdio.h"
#include "string.h"
#include "usbd_cdc_if.h"
#include "spi.h"

#include "drv_oled.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


__attribute__((aligned(32))) uint16_t adc_value[5];


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId KeyScanHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void oled_spi_tx_callback(uint8_t *data, uint32_t len);
void oled_gpio_rst_callback(uint8_t level);
void oled_gpio_dc_callback(uint8_t level);
void oled_delay_callback(uint32_t ms);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void KeyScanTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of KeyScan */
  osThreadDef(KeyScan, KeyScanTask, osPriorityNormal, 0, 128);
  KeyScanHandle = osThreadCreate(osThread(KeyScan), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */


    drv_oled_init(oled_spi_tx_callback, oled_gpio_rst_callback, oled_gpio_dc_callback, oled_delay_callback);


//    oled_fill(10, 10, 20, 20, 1);
//
//    oled_draw_char(30, 30, 'B', OLED_FONT_SIZE_1224);

//    oled_draw_number(10, 20, 123, 5, OLED_FONT_SIZE_0612);


    oled_draw_string(10, 20, "Hello World", OLED_FONT_SIZE_0612);

    oled_draw_string(10, 40, "Xiong Tao", OLED_FONT_SIZE_0612);

    oled_refresh_gram();


  int cnt = 0;

  /* Infinite loop */
  for(;;)
  {
      char buff[64];
      snprintf(buff, 64, "adc_value:%d %d %d %d %d\r\n", adc_value[0], adc_value[1], adc_value[2], adc_value[3], adc_value[4]);
//      while (USBD_BUSY == CDC_Transmit_FS(buff, strlen(buff))) {}

      HAL_ADC_Start_DMA(&hadc1, adc_value, 5);

      HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_KeyScanTask */
/**
* @brief Function implementing the KeyScan thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyScanTask */
void KeyScanTask(void const * argument)
{
  /* USER CODE BEGIN KeyScanTask */
  /* Infinite loop */
  for(;;)
  {
      int key_left = !HAL_GPIO_ReadPin(KEY_L_GPIO_Port, KEY_L_Pin);
      int key_right = !HAL_GPIO_ReadPin(KEY_R_GPIO_Port, KEY_R_Pin);
      int key_stick1 = !HAL_GPIO_ReadPin(KEY_STICK1_GPIO_Port, KEY_STICK1_Pin);
      int key_stick2 = !HAL_GPIO_ReadPin(KEY_STICK2_GPIO_Port, KEY_STICK2_Pin);

      if (key_left || key_right || key_stick1 || key_stick2) {
          HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
      } else {
          HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
      }

    osDelay(1);
  }
  /* USER CODE END KeyScanTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {

        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    }
}


void oled_spi_tx_callback(uint8_t *data, uint32_t len)
{
    HAL_SPI_Transmit(&hspi2, data, len, 100);
}

void oled_gpio_rst_callback(uint8_t level)
{
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, level);
}

void oled_gpio_dc_callback(uint8_t level)
{
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, level);
}

void oled_delay_callback(uint32_t ms)
{
    osDelay(pdMS_TO_TICKS(ms));
}




/* USER CODE END Application */

