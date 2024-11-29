/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "chip_config.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */


void app_init() {
  // torch::executor::runtime_init();
}

#define CACHELINE         64
#define REGION_SIZE_LINES 8
__attribute__((aligned(CACHELINE))) uint64_t mem1[CACHELINE / sizeof(uint64_t) * REGION_SIZE_LINES];
__attribute__((aligned(CACHELINE))) uint64_t mem2[CACHELINE / sizeof(uint64_t) * REGION_SIZE_LINES];
__attribute__((aligned(CACHELINE))) uint8_t operands[64];
#define DMA1 ((volatile DMA_Type*) 0x8800000)

void app_main() {
  uint64_t mhartid = READ_CSR("mhartid");
  Status status;

  while(1) {
    // printf("Hello Jasmine, this is hart ID %d speaking :P\r\n", mhartid);
    // printf("--- Test: Initializing memory\r\n");

    // void* src_addr = mem1;
    // void* dest_addr = mem2;
    // uint64_t stride = CACHELINE;

    // printf("--- Test: Waiting for DMA ready\r\n");

    // status = dma_await_result(DMA1); // wait for ready

    // printf("--- Test: Performing MAC with status %i\r\n", status);

    // dma_init_MAC(DMA1, src_addr, operands, stride, 33);

    // // DMA1->SRC_ADDR = src_addr;
    // // DMA1->DEST_ADDR = dest_addr;
    // // DMA1->SRCSTRIDE = stride;
    // // DMA1->MODE = DMA_Operation.MODE_MAC; 
    // // DMA1->COUNT = 33;

    // // wait for peripheral to complete (should hang otherwise)
    // status = dma_await_result(DMA1);
    // if (dma_operation_errored(DMA1)) {
    //   printf("Error detection success, state set to %i\r\n", status);
    // } else {
    //   printf("No error detected, fail, state set to %i\r\n", status);
    // }

    // printf("--- Test: Test complete. Retrying.\r\n");

  }
}
/* USER CODE END PUC */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* MCU Configuration--------------------------------------------------------*/

  /* Configure the system clock */
  /* Configure the system clock */
  
  /* USER CODE BEGIN SysInit */
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */  
  /* USER CODE BEGIN Init */
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    app_main();
    return 0;
  }
  /* USER CODE END WHILE */
}

/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {
  while (1) {
   asm volatile ("wfi");
  }
}