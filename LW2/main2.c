#include "stm32f4xx.h"

void delay_scan(volatile uint32_t t) { while (t--) __NOP(); }

int8_t digit_code[10] = {
 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

// Массив для основного числа (например, 2026)
int number[4] = {2, 0, 2, 6};

int main(void) {
    // Включаем тактирование портов A, B и C
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // Настройка PA0-PA7 (сегменты)
    for (int i = 0; i <= 7; i++) {
        GPIOA->MODER &= ~(0x3U << (i * 2U));
        GPIOA->MODER |=  (0x1U << (i * 2U));
    }
    
    // Настройка PC0-PC3 (разряды)
    for (int i = 0; i <= 3; i++) {
        GPIOC->MODER &= ~(0x3U << (i * 2U));
        GPIOC->MODER |=  (0x1U << (i * 2U));
    }

    // --- НОВОЕ: Настройка PB0 как ВХОД с подтяжкой вверх ---
    // Очищаем биты для PB0 (00 = Вход)
    GPIOB->MODER &= ~(0x3U << (0 * 2U));
    // Включаем подтяжку вверх (01 = Pull-up), чтобы без нажатия было 1
    GPIOB->PUPDR &= ~(0x3U << (0 * 2U));
    GPIOB->PUPDR |=  (0x1U << (0 * 2U));

    while (1) {
        // --- НОВОЕ: Проверка кнопки ---
        // Если на PB0 логический 0 (кнопка нажата)...
        if ((GPIOB->IDR & (1U << 0U)) == 0) {
            // ...показываем год рождения (например, 2005)
            number[0] = 2;
            number[1] = 0;
            number[2] = 0;
            number[3] = 5;
        } else {
            // ...иначе показываем текущий год (2026)
            number[0] = 2;
            number[1] = 0;
            number[2] = 2;
            number[3] = 6;
        }

        // Твой цикл динамической индикации (без изменений)
        for (int pos = 0; pos < 4; pos++) {
            GPIOC->BSRR = (0x000F << 16U); // Гасим все разряды
            GPIOA->ODR = digit_code[number[pos]]; // Подаем сегменты
            GPIOC->BSRR = (1U << pos); // Включаем нужный разряд
            delay_scan(2000); // Задержка
        }
    }
}