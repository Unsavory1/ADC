#include "stm32f10x.h"

#define Ctrl_Countflag (1 << 16)
#define EOC_flag 0x02

int adc_val = 0;

void ADC_Config(void);
void GPIO_Config(void);
void delay(int ms);
void PWM_Config(void);
void ADC1_2_IRQHandler(void);

int main() {

  ADC_Config();
  GPIO_Config();
  PWM_Config();

  while (1) {
    ADC1->CR2 |= (1 << 22);
    ADC1_2_IRQHandler(); // waits for conversion to complete
    int pwm_value = (adc_val * TIM2->ARR) / 4095;
    TIM2->CCR1 = pwm_value;
  }
}

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
  RCC->APB2ENR |= (1 << 2); // Enable GPIOA clock
  RCC->APB2ENR |= (1 << 0); // Enable AFIO clock
  GPIOA->CRL |=
      (1 << 1) | (1 << 3); // Configure PA0 as alternate function push-pull
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
  ADC1->CR2 |= (1 << 2); // calibration
  delay(2);
}

void ADC1_2_IRQHandler(void) {
  if (ADC1->SR & EOC_flag) {
    adc_val = ADC1->DR; // clearing the eoc flag by reading the data register
  }
}
void PWM_Config(void) {
  RCC->APB1ENR |= (1 << 0); // Enable TIM2 clock
  TIM2->PSC = 72 - 1;       // Prescaler value
  TIM2->ARR = 1000 - 1;     // Auto-reload value
  TIM2->CCMR1 |= (6 << 4);  // PWM mode 1
  TIM2->CCER |= (1 << 0);   // Enable TIM2 channel 1
  TIM2->CR1 |= (1 << 7);    // Auto-reload preload enable
  TIM2->CR1 |= (1 << 0);    // Enable TIM2
}