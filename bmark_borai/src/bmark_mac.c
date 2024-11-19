#include "main.h"

#define CACHELINE         64
#define REGION_SIZE_LINES 16 
#define STRIDE (4 * CACHELINE)

__attribute__((aligned(CACHELINE))) int8_t mem1[REGION_SIZE_LINES][CACHELINE / sizeof(int8_t)];
#define DMA1 ((volatile DMA_Type*) 0x8800000)

void run_bmark_mac() {
  Status status;
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

  printf("--- Test: Generating expected outputs using CPU");

  uint64_t start_time_cpu = READ_CSR("mcycle");
  for (size_t i = 0; i < count; i++) {
    int32_t sum = 0;
    for (size_t j = 0; j < sizeof(operandReg)/sizeof(uint8_t); j++) {
      sum += (int16_t)operandReg[j] * (int16_t)mem1[i*(STRIDE/CACHELINE)][j];
    }
    expected[i] = saturate(sum);
  }
  uint64_t end_time_cpu = READ_CSR("mcycle");

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

  printf("Test complete.\r\n", );
  printf("\tNaive cycle count:\t%lu\r\n", (end_time_cpu - start_time_cpu));
  printf("\tAccelerator cycle count:\t%lu\r\n", (end_time - start_time));
}