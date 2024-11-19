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

// void run_bmark_mac() {
//   __attribute__((aligned(CACHELINE))) int8_t mem1[REGION_SIZE_LINES][CACHELINE / sizeof(int8_t)];
//   __attribute__((aligned(CACHELINE))) int16_t dst_out[32];
//   uint64_t stride = (4 * CACHELINE);

//   Status status;
//   printf("--- Test: Initializing memory\r\n");
//   for (size_t i = 0; i < REGION_SIZE_LINES; i++) {
//     for (size_t j = 0; j < CACHELINE/sizeof(int8_t); j++) {
//       mem1[i][j] = i + j;
//     }
//   }

//   int8_t operandReg[sizeof(DMA1->OPERAND_REG)/sizeof(uint8_t)];
//   for (size_t i = 0; i < sizeof(operandReg)/sizeof(uint8_t); i++) {
//     operandReg[i] = (i + 3) * (i & 1 ? -1 : 1);
//   }

//   if (sizeof(operandReg) != 64) {
//     printf("operand reg size error! Not 64.\r\n");
//     return;
//   }
  
//   if (stride % CACHELINE != 0) {
//     printf("Stride not aligned!\r\n");
//     return;
//   }
//   uint32_t count = REGION_SIZE_LINES / (stride / CACHELINE);
//   int16_t expected[sizeof(DMA1->DEST_REG)/sizeof(int16_t)];

//   printf("--- Test: Generating expected outputs using CPU\r\n");

//   uint64_t start_time_cpu = READ_CSR("mcycle");
//   for (size_t i = 0; i < count; i++) {
//     int32_t sum = 0;
//     for (size_t j = 0; j < sizeof(operandReg)/sizeof(uint8_t); j++) {
//       sum += (int16_t)operandReg[j] * (int16_t)mem1[i*(stride/CACHELINE)][j];
//     }
//     expected[i] = saturate(sum);
//   }
//   uint64_t end_time_cpu = READ_CSR("mcycle");

//   void* src_addr = mem1;

//   printf("--- Test: Waiting for DMA ready\r\n");

//   status = dma_await_result(DMA1); // wait for ready

//   printf("--- Test: Performing MAC with initial status %i\r\n", status);
//   printf("--- Test: \tsrc_addr: %p\r\n", src_addr);
//   printf("--- Test: \toperands: %p\r\n", operandReg);
//   printf("--- Test: \tstride:   %i\r\n", stride);
//   printf("--- Test: \tcount:    %i\r\n", count);

//   uint64_t start_time = READ_CSR("mcycle");
//   dma_init_MAC(DMA1, src_addr, operandReg, stride, count);

//   // wait for peripheral to complete
//   status = dma_get_MAC_result(DMA1, dst_out, count);
//   uint64_t end_time = READ_CSR("mcycle");
  
//   if (status != DMA_OK) {
//     printf("Error detected, status set to %i\r\n", status);
//   } else {
//     printf("No error detected, state set to %i\r\n", status);
//   }

//   printf("Value Comparison Dump:\r\n");
//   for (size_t i = 0; i < count; i++) {
//     if (expected[i] != dst_out[i]) {
//       printf("\t[INVALID D:] ");
//     } else {
//       printf("\t[CORRECT :D] ");
//     }
//     printf("Expected %d at index %ld, got %d\r\n", expected[i], i, dst_out[i]);
    
//   }
//   printf("Dumping DEST_REG values...\r\n");

//   for (size_t i = 0; i < 8; i++) {
//     printf("\t%#010x\r\n", DMA1->DEST_REG[i]);
//   }

//   printf("Test complete.\r\n");
//   printf("\tNaive cycle count:\t%lu\r\n", (end_time_cpu - start_time_cpu));
//   printf("\tAccelerator cycle count:\t%lu\r\n", (end_time - start_time));
// }

void run_bmark_mac(volatile uint32_t region_size_lines) {
  printf("--- Test: Running test\r\n");
  // __attribute__((aligned(CACHELINE))) int8_t mem1[region_size_lines][CACHELINE / sizeof(int8_t)];
  int8_t *mem1 = 0x90000000;
  printf("--- Test: Running 2\r\n");
  uint64_t stride = (4 * CACHELINE);

  Status status;
  printf("--- Test: Initializing memory\r\n");
  for (size_t i = 0; i < region_size_lines; i++) {
    for (size_t j = 0; j < CACHELINE/sizeof(int8_t); j++) {
      printf("--- Test: Loop i=%u j=u\r\n", i, j);
      mem1[i * region_size_lines + j] = i + j;
    }
  }

  int8_t operandReg[sizeof(DMA1->OPERAND_REG)/sizeof(uint8_t)];
  for (size_t i = 0; i < sizeof(operandReg)/sizeof(uint8_t); i++) {
    operandReg[i] = (i + 3) * (i & 1 ? -1 : 1);
  }

  if (sizeof(operandReg) != 64) {
    printf("operand reg size error! Not 64.\r\n");
    return;
  }
  
  if (stride % CACHELINE != 0) {
    printf("Stride not aligned!\r\n");
    return;
  }
  uint32_t count = region_size_lines / (stride / CACHELINE);
  int16_t expected[sizeof(DMA1->DEST_REG)/sizeof(int16_t)];

  printf("--- Test: Generating expected outputs using CPU\r\n");

  uint64_t start_time_cpu = READ_CSR("mcycle");
  for (size_t i = 0; i < count; i++) {
    int32_t sum = 0;
    for (size_t j = 0; j < sizeof(operandReg)/sizeof(uint8_t); j++) {
      // sum += (int16_t)operandReg[j] * (int16_t)mem1[i*(stride/CACHELINE)][j];
      sum += (int16_t)operandReg[j] * (int16_t)mem1[i*(stride/CACHELINE) * region_size_lines + j];
    }
    expected[i] = saturate(sum);
  }
  uint64_t end_time_cpu = READ_CSR("mcycle");

  void* src_addr = mem1;

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
    if (expected[i] != ((volatile int16_t *)DMA1->DEST_REG)[i]) {
      printf("\t[INVALID D:] ");
    } else {
      printf("\t[CORRECT :D] ");
    }
    printf("Expected %d at index %ld, got %d\r\n", expected[i], i, ((volatile int16_t *)DMA1->DEST_REG)[i]);
    
  }
  printf("Dumping DEST_REG values...\r\n");

  for (size_t i = 0; i < 8; i++) {
    printf("\t%#010x\r\n", DMA1->DEST_REG[i]);
  }

  printf("Test complete.\r\n");
  printf("\tNaive cycle count:\t%lu\r\n", (end_time_cpu - start_time_cpu));
  printf("\tAccelerator cycle count:\t%lu\r\n", (end_time - start_time));
}

void run_bmark_mac_unaligned() {
  // __attribute__((aligned(CACHELINE))) int8_t mem1[REGION_SIZE_LINES][CACHELINE / sizeof(int8_t)];
  // printf("--- Test: Initializing memory\r\n");
  // for (size_t i = 0; i < REGION_SIZE_LINES; i++) {
  //   for (size_t j = 0; j < CACHELINE/sizeof(int8_t); j++) {
  //     mem1[i][j] = i + j;
  //   }
  // }

  // int8_t operandReg[sizeof(DMA1->OPERAND_REG)/sizeof(uint8_t)];
  // for (size_t i = 0; i < sizeof(operandReg)/sizeof(uint8_t); i++) {
  //   operandReg[i] = (i + 3) * (i & 1 ? -1 : 1);
  // }

  // _Static_assert(sizeof(operandReg) == 64, "opreg size");

  // _Static_assert(STRIDE % CACHELINE == 0, "stride not aligned");
  // uint32_t count = REGION_SIZE_LINES / (STRIDE / CACHELINE);
  // int16_t expected[sizeof(DMA1->DEST_REG)/sizeof(int16_t)];
  // for (size_t i = 0; i < count; i++) {
  //   int32_t sum = 0;
  //   for (size_t j = 0; j < sizeof(operandReg)/sizeof(uint8_t); j++) {
  //     sum += (int16_t)operandReg[j] * (int16_t)mem1[i*(STRIDE/CACHELINE)][j];
  //   }
  //   expected[i] = saturate(sum);
  // }

  // void* src_addr = mem1;
  // uint64_t stride = STRIDE;

  // printf("--- Test: Waiting for DMA ready\r\n");

  // status = dma_await_result(DMA1); // wait for ready

  // printf("--- Test: Performing MAC with initial status %i\r\n", status);
  // printf("--- Test: \tsrc_addr: %p\r\n", src_addr);

  // printf("--- Test: \toperands: %p\r\n", operandReg);
  // printf("--- Test: \tstride:   %i\r\n", stride);
  // printf("--- Test: \tcount:    %i\r\n", count);
  // uint64_t start_time = READ_CSR("mcycle");
  // dma_init_MAC(DMA1, src_addr, operandReg, stride, count);

  // // memcpy(DMA1->OPERAND_REG, operandReg, sizeof(operandReg));
  // // __asm__ ("" ::: "memory");

  // // wait for peripheral to complete
  // status = dma_await_result(DMA1);
  // uint64_t end_time = READ_CSR("mcycle");
  // if (dma_operation_errored(DMA1)) {
  //   printf("Error detected, status set to %i\r\n", status);
  // } else {
  //   printf("No error detected, state set to %i\r\n", status);
  // }

  // printf("Value Comparison Dump:\r\n");
  // for (size_t i = 0; i < count; i++) {
  //   //if (expected[i] != ((volatile int16_t *)DMA1->DEST_REG)[i]) {
  //     printf("\tExpected %d at index %ld, got %d\r\n", expected[i], i, ((volatile int16_t *)DMA1->DEST_REG)[i]);
  //   //}
  // }
  // printf("Dumping DEST_REG values...\r\n");

  // for (size_t i = 0; i < 8; i++) {
  //   printf("\t%#010x\r\n", DMA1->DEST_REG[i]);
  // }
  

  // printf("Test complete, operation cycle count: %lu\r\n", (end_time - start_time));
}

void run_bmark_memcpy(int32_t region_size_lines) {  // region size lines default 8
  Status status;
  __attribute__((aligned(CACHELINE))) uint64_t mem1[CACHELINE / sizeof(uint64_t) * region_size_lines];
  __attribute__((aligned(CACHELINE))) uint64_t mem2[CACHELINE / sizeof(uint64_t) * region_size_lines];
  __attribute__((aligned(CACHELINE))) uint64_t mem2_cpu[CACHELINE / sizeof(uint64_t) * region_size_lines];

  puts("--- Test: Initializing memory\r\n");

  for (size_t i = 0; i < sizeof(mem1)/sizeof(uint64_t); i++) {
    mem1[i] = i;
  }

  void* src_addr = mem1;
  void* dest_addr = mem2;
  uint64_t stride = CACHELINE;
  uint32_t count = region_size_lines;

  printf("--- Test: Generating time comparison using CPU\r\n");

  uint64_t start_time_cpu = READ_CSR("mcycle");
  for (size_t i = 0; i < sizeof(mem1)/sizeof(uint64_t); i++) {
    mem2_cpu[i] = mem1[i];
  }
  uint64_t end_time_cpu = READ_CSR("mcycle");

  printf("--- Test: Verifying CPU memcpy\r\n");

  for (size_t i = 0; i < sizeof(mem1)/sizeof(uint64_t); i++) {
    if (mem2_cpu[i] != mem1[i]) {
      printf("!\r\n");

      printf("Expected %d at index %ld, got %d\r\n", mem1[i], i, mem2_cpu[i]);
      return;
    } else {
      printf(".");
    }
  }

  printf("--- Test: Waiting for DMA ready\r\n");

  status = dma_await_result(DMA1); // wait for ready

  printf("--- Test: Performing MEMCPY with initial status %i\r\n", status);
  printf("--- Test: \tsrc_addr: %p\r\n", src_addr);
  printf("--- Test: \tdest_addr: %p\r\n", dest_addr);
  printf("--- Test: \tstride:   %i\r\n", stride);
  printf("--- Test: \tcount:    %i\r\n", count);

  printf("--- Test: Performing MEMCPY\r\n");

  dma_init_memcpy(DMA1, src_addr, dest_addr, stride, count);

  status = dma_await_result(DMA1);
  uint64_t end_time = READ_CSR("mcycle");
  if (dma_operation_errored(DMA1)) {
    printf("Error detected, status set to %i\r\n", status);
  } else {
    printf("No error detected, state set to %i\r\n", status);
  }

  printf("--- Test: Verifying accelerator memcpy\r\n");

  for (size_t i = 0; i < sizeof(mem1)/sizeof(uint64_t); i++) {
    if (mem2[i] != mem1[i]) {
      printf("!\r\n");
      printf("Expected %d at index %ld, got %d\r\n", mem1[i], i, mem2[i]);
    } else {
      printf(".");
    }
  }

  puts("Test complete.\r\n");
}

void run_bmark_error(uint32_t region_size_lines) { // default 8
  Status status;
  __attribute__((aligned(CACHELINE))) uint64_t mem1[CACHELINE / sizeof(uint64_t) * region_size_lines];
  // __attribute__((aligned(CACHELINE))) uint64_t mem2[CACHELINE / sizeof(uint64_t) * region_size_lines];
  __attribute__((aligned(CACHELINE))) int8_t operands[64];
  __attribute__((aligned(CACHELINE))) int16_t dst_out[32];


  puts("--- Test: Initializing memory\r");

  void* src_addr = mem1;
  // void* dest_addr = mem2;
  uint64_t stride = CACHELINE;
  uint32_t count = 33;

  puts("--- Test: Waiting for DMA Ready\r");

  status = dma_await_result(DMA1);

  printf("--- Test: Performing MAC with initial status %i\r\n", status);
  printf("--- Test: \tsrc_addr: %p\r\n", src_addr);
  printf("--- Test: \toperands: %p\r\n", operands);
  printf("--- Test: \tstride:   %i\r\n", stride);
  printf("--- Test: \tcount:    %i\r\n", count);

  dma_init_MAC(DMA1, src_addr, operands, stride, count);

  // wait for peripheral to complete (should hang otherwise)
  printf("--- Test: Waiting for result\r\n");
  status = dma_get_MAC_result(DMA1, dst_out, count);

  for (size_t i = 0; i < 32; i++) {
    if (dst_out[i] != -1) {
      printf("!\r\n");
      printf("Expected -1 at index %ld, got %d\r\n", i, dst_out[i]);
    } else {
      printf(".");
    }
  }

  if (status != DMA_OK) {
    printf("\r\nError detection success, state set to %i\r\n", status);
  } else {
    printf("\r\nNo error detected, fail, state set to %i\r\n", status);
  }
  printf("Test complete.\r\n");
}

void app_main() {
  uint64_t mhartid = READ_CSR("mhartid");
  Status status;
  printf("Hello Jasmine, this is hart ID %d speaking :P\r\n", mhartid);
  while(1) {
    
    printf("*--- Jasmine's Super Cool Bearly Tester ---*\r\n", mhartid);
    printf("Available tests:\r\n");
    printf("\t0: run_bmark_mac\r\n");
    printf("\t1: run_bmark_mac_unaligned\r\n");
    printf("\t2: run_bmark_memcpy\r\n");
    printf("\t3: run_bmark_error\r\n");
    printf("Enter the ID for the test you wish to run:\r\n");
    int8_t entered_id = 0;
    uart_receive(UART0, &entered_id, 1, 100);
    printf("User input: %c\r\n", entered_id);
    switch (entered_id) {
      case 48:
        run_bmark_mac(128);
        break;
      case 49:
        run_bmark_mac_unaligned();
        break;
      case 50:
        run_bmark_memcpy(8);
        break;
      case 51:
        run_bmark_error(8);
        break;
      default:
        printf("Invalid input.\r\n");
    }
    entered_id = 0;
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