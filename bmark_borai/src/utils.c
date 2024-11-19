/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : utils.c
  * @brief          : Testing utilities
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

#include "utils.h"

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
