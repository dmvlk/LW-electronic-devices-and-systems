#include "stm32f4xx.h"

void delay_scan(volatile uint32_t t) { while (t--) __NOP(); }

int8_t digit_code[10] = {
 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

int number[4] = {2, 0, 2, 6};

int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    for (int i = 0; i <= 7; i++) {
        GPIOA->MODER &= ~(0x3U << (i * 2U));
        GPIOA->MODER |=  (0x1U << (i * 2U));
    }
    for (int i = 0; i <= 3; i++) {
        GPIOC->MODER &= ~(0x3U << (i * 2U));
        GPIOC->MODER |=  (0x1U << (i * 2U));
    }

    while (1) {
        for (int pos = 0; pos < 4; pos++) {
            GPIOC->BSRR = (0x000F << 16U);
            GPIOA->ODR = digit_code[number[pos]];
            GPIOC->BSRR = (1U << pos);
            delay_scan(2000);
        }
    }
}