// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"


void initDelayTIM(TIM_TypeDef * TIMx){
  // Set PSC to give 1 ms time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e3));

  // Set PSC division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update PSC value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; 
}

void initTIM(TIM_TypeDef * TIMx){
  // Set PSC to give 1 us time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e6));

  // Set PSC division factor
  TIMx->PSC = (psc_div - 1);

  // Generate an update event to update PSC value
  TIMx->EGR |= 1;

  // Enable counter
  TIMx->CR1 |= 1; 
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR = ms;     // Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}