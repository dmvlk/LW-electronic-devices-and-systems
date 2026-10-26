#include "stm32f4xx.h"

void delay(volatile uint32_t t) {
    while (t--) __NOP();
}

int main(void) {
    // 1. Включаем тактирование порта А
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // 2. Настройка PA0–PA7 как выходов
    for (int i = 0; i < 4; i++) { // Если используете 8 светодиодов, оставьте 8. Если 4 - поставьте 4.
        GPIOA->MODER &= ~(0x3U << (i * 2U));
        GPIOA->MODER |= (0x1U << (i * 2U)); // Выход
        GPIOA->OTYPER &= ~(1U << i);         // Push-pull
        GPIOA->OSPEEDR &= ~(0x3U << (i * 2U)); // Низкая скорость
        GPIOA->PUPDR &= ~(0x3U << (i * 2U));   // Без подтяжки
    }

    uint8_t pattern = 0x01U; // Начинаем с PA0 (0000 0001)

    while (1) {
        // Записываем шаблон в младшие 8 бит ODR, сохраняя старшие
        GPIOA->ODR = (GPIOA->ODR & 0xFF00U) | pattern;
        delay(200000); // Задержка

        pattern <<= 1; // Сдвигаем единицу влево
        if (pattern == 0U) {
            pattern = 0x01U; // Возвращаемся к PA0
        }
    }
}