#include "stm32f4xx.h"

// Переменная для хранения счетчика (глобальная, чтобы таймер её видел)
volatile uint32_t seconds = 0; 

// Функция задержки для индикации
void delay_scan(volatile uint32_t t) { while (t--) __NOP(); }

// Таблица кодов цифр
int8_t digit_code[10] = {
 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

// Массив для цифр числа
int number[4] = {0, 0, 0, 0};

// --- Настройка SysTick на 1 секунду ---
// Частота ядра STM32F407 по умолчанию 168 МГц (168 000 000 тактов в секунду)
void init_systick(void) {
    // Загружаем число тактов для 1 секунды
    SysTick->LOAD = 168000000 - 1; 
    // Сбрасываем текущий счетчик
    SysTick->VAL = 0;
    // Включаем таймер: бит 2 (CLKSOURCE) = 1 (такт ядра), бит 0 (ENABLE) = 1, 
    // бит 1 (TICKINT) = 1 (разрешаем прерывания)
    SysTick->CTRL = 0x00000007; 
}

// --- Обработчик прерывания SysTick ---
// Эта функция вызывается автоматически каждую секунду
void SysTick_Handler(void) {
    seconds++; // Увеличиваем счетчик
    if (seconds > 9999) {
        seconds = 0; // Сброс после 9999
    }
}

int main(void) {
    // 1. Включаем тактирование портов A и C
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    // 2. Настройка PA0-PA7 (сегменты)
    for (int i = 0; i <= 7; i++) {
        GPIOA->MODER &= ~(0x3U << (i * 2U));
        GPIOA->MODER |=  (0x1U << (i * 2U));
    }
    
    // 3. Настройка PC0-PC3 (разряды)
    for (int i = 0; i <= 3; i++) {
        GPIOC->MODER &= ~(0x3U << (i * 2U));
        GPIOC->MODER |=  (0x1U << (i * 2U));
    }

    // 4. Запускаем таймер
    init_systick();

    while (1) {
        // 5. Раскладываем число seconds (например, 42) на цифры: 0, 0, 4, 2
        number[0] = (seconds / 1000) % 10;
        number[1] = (seconds / 100) % 10;
        number[2] = (seconds / 10) % 10;
        number[3] = (seconds % 10);

        // 6. Динамическая индикация (без изменений)
        for (int pos = 0; pos < 4; pos++) {
            GPIOC->BSRR = (0x000F << 16U); // Гасим разряды
            GPIOA->ODR = digit_code[number[pos]]; // Подаем цифру
            GPIOC->BSRR = (1U << pos); // Включаем разряд
            delay_scan(2000); // Задержка (очень короткая!)
        }
    }
}