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
#define REGION_SIZE_LINES 16 
#define STRIDE (4 * CACHELINE)

__attribute__((aligned(CACHELINE))) int8_t mem1[REGION_SIZE_LINES][CACHELINE / sizeof(int8_t)];
#define DMA1 ((volatile DMA_Type*) 0x8800000)

int16_t saturate(int32_t x) {
  int16_t max = (1 << 15) - 1;
  int16_t min = -(1 << 15);
  if (x > max) {
    return max;
  } else if (x < min) {
    return min;
  } else {
    return x;
  }
}


void app_main() {
  uint64_t mhartid = READ_CSR("mhartid");
  Status status;

  while(1) {
    printf("Hello Jasmine, this is hart ID %d speaking :P\r\n", mhartid);
    printf("--- Test: Initializing memory\r\n");
    for (size_t i = 0; i < REGION_SIZE_LINES; i++) {
      for (size_t j = 0; j < CACHELINE/sizeof(int8_t); j++) {
        mem1[i][j] = i + j;
      }
    }

    int8_t operandReg[sizeof(DMA1->OPERAND_REG)/sizeof(uint8_t)];
    for (size_t i = 0; i < sizeof(operandReg)/sizeof(uint8_t); i++) {
      operandReg[i] = (i + 3) * (i & 1 ? -1 : 1);
    }

    _Static_assert(sizeof(operandReg) == 64, "opreg size");

    _Static_assert(STRIDE % CACHELINE == 0, "stride not aligned");
    uint32_t count = REGION_SIZE_LINES / (STRIDE / CACHELINE);
    int16_t expected[sizeof(DMA1->DEST_REG)/sizeof(int16_t)];
    for (size_t i = 0; i < count; i++) {
      int32_t sum = 0;
      for (size_t j = 0; j < sizeof(operandReg)/sizeof(uint8_t); j++) {
        sum += (int16_t)operandReg[j] * (int16_t)mem1[i*(STRIDE/CACHELINE)][j];
      }
      expected[i] = saturate(sum);
    }
 
    void* src_addr = mem1;
    uint64_t stride = STRIDE;

    printf("--- Test: Waiting for DMA ready\r\n");

    status = dma_await_result(DMA1); // wait for ready

    printf("--- Test: Performing MAC with initial status %i\r\n", status);
    printf("--- Test: \tsrc_addr: %p\r\n", src_addr);
    printf("--- Test: \toperands: %p\r\n", operandReg);
    printf("--- Test: \tstride:   %i\r\n", stride);
    printf("--- Test: \tcount:    %i\r\n", count);
    uint64_t start_time = READ_CSR("mcycle");
    dma_init_MAC(DMA1, src_addr, operandReg, stride, count);

    // memcpy(DMA1->OPERAND_REG, operandReg, sizeof(operandReg));
    // __asm__ ("" ::: "memory");

    // wait for peripheral to complete
    status = dma_await_result(DMA1);
    uint64_t end_time = READ_CSR("mcycle");
    if (dma_operation_errored(DMA1)) {
      printf("Error detected, status set to %i\r\n", status);
    } else {
      printf("No error detected, state set to %i\r\n", status);
    }

    printf("Value Comparison Dump:\r\n");
    for (size_t i = 0; i < count; i++) {
      //if (expected[i] != ((volatile int16_t *)DMA1->DEST_REG)[i]) {
        printf("\tExpected %d at index %ld, got %d\r\n", expected[i], i, ((volatile int16_t *)DMA1->DEST_REG)[i]);
      //}
    }
    printf("Dumping DEST_REG values...\r\n");

    for (size_t i = 0; i < 8; i++) {
      printf("\t%#010x\r\n", DMA1->DEST_REG[i]);
    }
    

    printf("Test complete, operation cycle count: %lu\r\n", (end_time - start_time));
    msleep(10000);

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