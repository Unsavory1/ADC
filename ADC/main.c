#include "stm32f10x.h"

#define Ctrl_Countflag (1 << 16)

void ADC_Config(void);
void GPIO_Config(void);
void delay(int ms);
int main() {}

void delay(int ms) {
  SysTick->LOAD = 9000;
  SysTick->VAL = 0;
  SysTick->CTRL |= (1 << 0) | (1 << 2);

  for (int i = 0; i < ms; i++) {
    while ((SysTick->CTRL & Ctrl_Countflag) == 0) {
    }
  }
  SysTick->CTRL = 0;
}

void GPIO_Config(void) {
  RCC->APB2ENR |= (1 << 2); // enables pin A
  RCC->APB2ENR |= (1 << 0); // enables alternate io
  GPIOA->CRL |= 0;
}

void ADC_Config(void) {
  // applying adc prescaler before enabling clock
  RCC->CFGR |= (1 << 15); // divides it by 6
  RCC->APB2ENR |= (1 << 9);
  ADC1->CR1 |= (1 << 5);       // enables end of conversion interupt
  NVIC_EnableIRQ(ADC1_2_IRQn); // interrupt handler

  // setting samping rate
  ADC1->SMPR2 |= (1 << 17) | (1 << 16) | (1 << 15);
  ADC1->SQR3 |= (5 << 0); // setting channel
  ADC1->CR2 |=
      (1 << 0) | (1 << 1); // enabling adc and setting it to continous mode
  delay(1);
  ADC1->CR2 |= (1 << 2);
  delay(2);
}