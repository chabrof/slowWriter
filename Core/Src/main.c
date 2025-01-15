/* USER CODE BEGIN Header */
#define __WE_ACT_STUDIO_VERSION
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#ifdef __WE_ACT_STUDIO_VERSION
  // WeActStudio includes :
  #include "stdint.h"
  #include "stdio.h"
  #include "epaper.h"
  #include "bmp.h"

#else
  // Waveshare includes :
  #include "DEV_Config.h"
  #include "GUI_Paint.h"
  #include "imagedata.h"
  #include "Debug.h"
  #include <stdlib.h> // malloc() free()
  #include "EPD_4in2_V2.h"
  #include <string.h>
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */
uint8_t image_bw[EPD_W_BUFF_SIZE * EPD_H];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM8_Init(void);
static void MX_UART4_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
#ifdef __WE_ACT_STUDIO_VERSION
  void _mainWeActStudio(void);
#else
  void _mainWaveShare(void);
#endif
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// (Chabe) SWO printf config :
/*int _write(int file, char *ptr, int len) {
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++) {
    ITM_SendChar(*ptr++);
  }
  return len;
}*/

// (Chabe) USART printf configuration :
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM8_Init();
  MX_UART4_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  #ifdef __WE_ACT_STUDIO_VERSION
    _mainWeActStudio();
  #else
    _mainWaveShare();
  #endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      /*ITM_SendChar('a');
      ITM_SendChar('b');
      ITM_SendChar('c');
      ITM_SendChar('\n');*/

      HAL_GPIO_TogglePin (GPIOE, GPIO_PIN_3);
      HAL_Delay (4000);   /* Insert delay 100 ms */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x0;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim8, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIMEx_TISelection(&htim8, TIM_TIM8_TI1_COMP2, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI2_DC_Pin|SPI2_CS_Pin|SPI2_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_BUSY_Pin */
  GPIO_InitStruct.Pin = SPI2_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI2_BUSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI2_DC_Pin SPI2_CS_Pin SPI2_RESET_Pin */
  GPIO_InitStruct.Pin = SPI2_DC_Pin|SPI2_CS_Pin|SPI2_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#ifdef __WE_ACT_STUDIO_VERSION
  void _mainWeActStudio(void) {
    uint8_t text[20];

    HAL_Delay(1000);
    printf("WeActStudio start : \r\n");
    epd_init();
    epd_paint_clear(EPD_COLOR_WHITE);
    epd_paint_newimage(image_bw, EPD_W, EPD_H, EPD_ROTATE_270, EPD_COLOR_WHITE);
    epd_paint_selectimage(image_bw);
    epd_paint_clear(EPD_COLOR_WHITE);
    epd_paint_showPicture(
      (EPD_W - 250) / 2,
      (EPD_H - 122) / 2,
      250, 122,
      gImage_1,
      EPD_COLOR_WHITE);
    epd_displayBW(image_bw);
    //epd_enter_deepsleepmode(EPD_DEEPSLEEP_MODE1);

    HAL_Delay(2000);
    printf("... Partial display  \r\n");
    epd_init_partial();

    epd_paint_selectimage(image_bw);
    epd_paint_clear(EPD_COLOR_WHITE);

    epd_paint_showString(10, 0, (uint8_t *)&"4.2 Inch Epaper Module", EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);
    epd_paint_showString(10, 50, (uint8_t *)&"with 400 x 300 resolution", EPD_FONT_SIZE16x8, EPD_COLOR_BLACK);
    epd_paint_showString(10, 29, (uint8_t *)&"Designed By WeAct Studio", EPD_FONT_SIZE16x8, EPD_COLOR_BLACK);

    /*#if 0
      epd_paint_showString(10,100,(uint8_t *)&"CH32F103C8T6 Example",EPD_FONT_SIZE16x8,EPD_COLOR_BLACK);
    #else*/
    epd_paint_drawRectangle(10, EPD_W-20, EPD_H - 10, EPD_W-6, EPD_COLOR_BLACK, 1);
    epd_update();
    //#endif
    HAL_Delay(1500);
    /*epd_paint_clear(EPD_COLOR_WHITE);
    HAL_Delay(700);
    epd_paint_clear(EPD_COLOR_BLACK);
    HAL_Delay(700);
    epd_paint_clear(EPD_COLOR_WHITE);
    HAL_Delay(700);
    epd_paint_clear(EPD_COLOR_BLACK);
  */
    sprintf((char *)&text, ">> Partial Mode");
    epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);

    epd_displayBW_partial(image_bw);

    printf("Before loop.\r\n");
    HAL_Delay(500);

    for (int i = 123; i < 8 * 123; i += 123)
    {
      sprintf((char *)&text, ">> Num=%d     ", i);
      epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);

      epd_displayBW_partial(image_bw);

      HAL_Delay(100);
    }

    sprintf((char *)&text, ">> Hello World.");
    epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);
    epd_displayBW_partial(image_bw);

    HAL_Delay(1000);
    epd_paint_clear(EPD_COLOR_WHITE);
    epd_update();

    epd_enter_deepsleepmode(EPD_DEEPSLEEP_MODE1);

    printf("END ");
    printf("(WeActStudio Code)\r\n");
  }

#else

  int _mainWaveShare(void) {
    printf("EPD_4IN2_V2_test Demo\r\n");
    if(DEV_Module_Init()!=0){
      return -1;
    }

    printf("e-Paper Init and Clear...\r\n");
    EPD_4IN2_V2_Init();
    EPD_4IN2_V2_Clear();
    DEV_Delay_ms(500);

    //Create a new image cache
    UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 8 ): (EPD_4IN2_V2_WIDTH / 8 + 1)) * EPD_4IN2_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);

    #if 1  // show bmp
      printf("show window BMP-----------------\r\n");
      Paint_SelectImage(BlackImage);
      Paint_Clear(WHITE);
      Paint_DrawBitMap(gImage_4in2);
      EPD_4IN2_V2_Display(BlackImage);
      DEV_Delay_ms(2000);

    #endif

    #if 1  // show image for array
    //    EPD_4IN2_V2_Init_Fast(Seconds_1_5S);
      EPD_4IN2_V2_Init_Fast(Seconds_1S);
      printf("show image for array\r\n");
      Paint_SelectImage(BlackImage);
      Paint_Clear(WHITE);
      Paint_DrawBitMap(gImage_4in2);
      EPD_4IN2_V2_Display_Fast(BlackImage);
      DEV_Delay_ms(2000);
    #endif

    #if 1   // Drawing on the image

      EPD_4IN2_V2_Init();
      //1.Select Image
      printf("SelectImage:BlackImage\r\n");
      Paint_SelectImage(BlackImage);
      Paint_Clear(WHITE);

      // 2.Drawing on the image
      printf("Drawing:BlackImage\r\n");
      Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
      Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
      Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
      Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
      Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
      Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
      Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
      Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
      Paint_DrawString_CN(130, 0, " ���abc", &Font12CN, BLACK, WHITE);
      Paint_DrawString_CN(130, 20, "΢ѩ����", &Font24CN, WHITE, BLACK);

      printf("EPD_Display\r\n");
       // EPD_4IN2_V2_Display(BlackImage);
      EPD_4IN2_V2_Display(BlackImage);
      DEV_Delay_ms(2000);
    #endif

    #if 1
      printf("Partial refresh\r\n");
      Paint_NewImage(BlackImage, 200, 50, 0, WHITE);
      PAINT_TIME sPaint_time;
      sPaint_time.Hour = 12;
      sPaint_time.Min = 34;
      sPaint_time.Sec = 56;
      UBYTE num = 10;
      for (;;) {
          sPaint_time.Sec = sPaint_time.Sec + 1;
       if (sPaint_time.Sec == 60) {
         sPaint_time.Min = sPaint_time.Min + 1;
         sPaint_time.Sec = 0;
         if (sPaint_time.Min == 60) {
           sPaint_time.Hour =  sPaint_time.Hour + 1;
           sPaint_time.Min = 0;
           if (sPaint_time.Hour == 24) {
             sPaint_time.Hour = 0;
             sPaint_time.Min = 0;
             sPaint_time.Sec = 0;
           }
         }
       }
       Paint_Clear(WHITE);
       Paint_DrawTime(20, 10, &sPaint_time, &Font20, WHITE, BLACK);
       EPD_4IN2_V2_PartialDisplay(BlackImage, 80, 200, 280, 250);
       DEV_Delay_ms(500);//Analog clock 1s
       num = num - 1;
       if(num == 0) {
         break;
       }
       }
   #endif


   #if 1
       // EPD_4IN2_V2_Init();
     // EPD_4IN2_V2_Clear();
     EPD_4IN2_V2_Init_4Gray();
     printf("show Gray------------------------\r\n");
     free(BlackImage);
     BlackImage = NULL;
     Imagesize = ((EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 4 ): (EPD_4IN2_V2_WIDTH / 4 + 1)) * EPD_4IN2_V2_HEIGHT;
       if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
           printf("Failed to apply for black memory...\r\n");
           return -1;
       }
     Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);
     Paint_SetScale(4);
     Paint_Clear(WHITE);

     Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
       Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
       Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
       Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
       Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
       Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
       Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
       Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
       Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
       Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
       Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
       Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
       Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
       Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
       Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
       Paint_DrawString_CN(140, 0, "���abc", &Font12CN, GRAY1, GRAY4);
       Paint_DrawString_CN(140, 40, "���abc", &Font12CN, GRAY2, GRAY3);
       Paint_DrawString_CN(140, 80, "���abc", &Font12CN, GRAY3, GRAY2);
       Paint_DrawString_CN(140, 120, "���abc", &Font12CN, GRAY4, GRAY1);

       Paint_DrawString_CN(220, 0, "΢ѩ����", &Font24CN, GRAY1, GRAY4);
       Paint_DrawString_CN(220, 40, "΢ѩ����", &Font24CN, GRAY2, GRAY3);
       Paint_DrawString_CN(220, 80, "΢ѩ����", &Font24CN, GRAY3, GRAY2);
       Paint_DrawString_CN(220, 120, "΢ѩ����", &Font24CN, GRAY4, GRAY1);

     EPD_4IN2_V2_Display_4Gray(BlackImage);
     DEV_Delay_ms(2000);

     Paint_Clear(WHITE);
       Paint_DrawBitMap(gImage_4in2_4Gray);
       EPD_4IN2_V2_Display_4Gray(BlackImage);
     DEV_Delay_ms(2000);

   #endif

       EPD_4IN2_V2_Init();
       EPD_4IN2_V2_Clear();
       printf("Goto Sleep...\r\n");
       EPD_4IN2_V2_Sleep();
       free(BlackImage);
       BlackImage = NULL;
       DEV_Delay_ms(2000);//important, at least 2s
       // close 5V
       printf("close 5V, Module enters 0 power consumption ...\r\n");
       DEV_Module_Exit();

       return 0;
  }
#endif
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
