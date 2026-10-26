#include "stm32f4xx.h"

// ---------- Функция задержки ----------
void delay(volatile uint32_t t) {
    while (t--) __NOP();
}

int main(void) {
    // === 1. Включаем тактирование порта A ===
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // === 2. Настраиваем PA0, PA1, PA2, PA3 как выходы push-pull ===
    for (int i = 0; i < 4; i++) {
        // Очищаем двухбитное поле MODER для вывода i
        GPIOA->MODER  &= ~(0x3U << (i * 2U));
        // Устанавливаем режим "Выход" (01)
        GPIOA->MODER  |=  (0x1U << (i * 2U));

        // Тип выхода: push-pull (0)
        GPIOA->OTYPER &= ~(1U << i);

        // Низкая скорость (00)
        GPIOA->OSPEEDR &= ~(0x3U << (i * 2U));

        // Без подтяжки (00)
        GPIOA->PUPDR  &= ~(0x3U << (i * 2U));
    }

    // === 3. Основной цикл ===
    while (1) {
        // ФАЗА 1: зажечь PA1 и PA2, погасить PA0 и PA3
        // Устанавливаем PA1 и PA2 (младшие 16 бит BSRR)
        GPIOA->BSRR = (1U << 1U) | (1U << 2U);
        // Сбрасываем PA0 и PA3 (старшие 16 бит BSRR)
        GPIOA->BSRR = (1U << (0U + 16U)) | (1U << (3U + 16U));

        delay(500000);

        // ФАЗА 2: зажечь PA0 и PA3, погасить PA1 и PA2
        // Устанавливаем PA0 и PA3
        GPIOA->BSRR = (1U << 0U) | (1U << 3U);
        // Сбрасываем PA1 и PA2
        GPIOA->BSRR = (1U << (1U + 16U)) | (1U << (2U + 16U));

        delay(500000);
    }
}