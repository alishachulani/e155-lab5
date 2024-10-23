

#include "/Users/alishachulani/Desktop/STM32L432KC.h" // your path might change depending on where your files are
#include <stm32l432xx.h>
#include <stdio.h>

// Define pins and timers
#define ASIGNAL_PIN PA9
#define BSIGNAL_PIN PA6
#define DELAY_TIM TIM2
#define COUNT_TIM TIM6

int delta = 0;
int off = 1;


// Interrupts
void EXTI9_5_IRQHandler(void){
    // Initialize measurement variables
    int a = digitalRead(ASIGNAL_PIN); 
    int b = digitalRead(BSIGNAL_PIN);

    if (EXTI->PR1 & (1 << 6)){
        // Clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 6);
        off = 0;
        
        // If both signals are the same calculate delta
        if(((b==1) && (a==1)) || ((b==0) && (a==0))){
          delta = -COUNT_TIM->CNT;
        }
        // reset the clock
        COUNT_TIM->CNT = 0;
    }

     if (EXTI->PR1 & (1 << 9)){
        // Clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << 9);
        off = 0;

        // if both signals are the same calculate delta
        if(((b==1) && (a==1)) || ((b==0) && (a==0))){
          delta = COUNT_TIM->CNT;
        }

        // reset the clock
        COUNT_TIM->CNT = 0;
    }
}

int main(void) {
    // Enable pins as inputs
    gpioEnable(GPIO_PORT_A);
    pinMode(ASIGNAL_PIN, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD9, 0b01); 

    pinMode(BSIGNAL_PIN, GPIO_INPUT);
    GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD6, 0b01); 

    // Initialize timers
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    initDelayTIM(DELAY_TIM);

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    initTIM(COUNT_TIM);
    
    // Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Configure EXTICR for the input button interrupt
    SYSCFG->EXTICR[2] |= _VAL2FLD(SYSCFG_EXTICR3_EXTI9, 0b000); // Select PA0
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI6, 0b000); // Select PA1
    __enable_irq();

    // Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(ASIGNAL_PIN));
    EXTI->IMR1 |= (1 << gpioPinOffset(BSIGNAL_PIN)); 

    // Enable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ASIGNAL_PIN));
    EXTI->RTSR1 |= (1 << gpioPinOffset(BSIGNAL_PIN));

    // Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(ASIGNAL_PIN));// Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(BSIGNAL_PIN));// Enable falling edge trigger

    // Turn on EXTI interrupt in NVIC_ISER.+-d
    NVIC->ISER[0] |= (1 << EXTI9_5_IRQn);
    
    // Initialize rpm variables 
    double rpm1 = 0;
    double rpm2 = 0;
    double rpm3 = 0;
    double rpm4 = 0;
    double rpm = 0;
    while(1){  
  
        delay_millis(DELAY_TIM, 250);

        // if clock is not reset for a long time, then the motor is not turning, toggle off
        if(COUNT_TIM->CNT > 45000){
          off = 1;
        }
        
        // if off is 1, then motor is not turning and rpm is 0
        if(off){
          rpm = 0;
        
        // else calculations for motor speed
        }else {
          if(rpm == 0){
            rpm1 = 1/(double)(120*delta*4/1000000.0);
            rpm2 = rpm1;
            rpm3 = rpm1;
            rpm4 = rpm1;
          } else {
            rpm1 = rpm2;
            rpm2 = rpm3;
            rpm3 = rpm4;
            rpm4 = 1/(double)(120*delta*4/1000000.0);
          }
          rpm = (rpm1+rpm2+rpm3+rpm4)/4;
        }
        
        printf("Revolutions per Second: %f\n", rpm);
        
    }

}
