#include "stm32f4xx.h"

void delay_scan(volatile uint32_t t) { while (t--) __NOP(); }

int8_t digit_code[10] = {
 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

int number[4] = {2, 0, 2, 6};

int main(void) {
    // Включаем тактирование портов A, B и C
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // Настройка PA0-PA7 (сегменты a-g + dp)
    for (int i = 0; i <= 7; i++) {
        GPIOA->MODER &= ~(0x3U << (i * 2U));
        GPIOA->MODER |=  (0x1U << (i * 2U));
    }
    
    // Настройка PC0-PC3 (разряды)
    for (int i = 0; i <= 3; i++) {
        GPIOC->MODER &= ~(0x3U << (i * 2U));
        GPIOC->MODER |=  (0x1U << (i * 2U));
    }

    // Настройка PB0 как вход с подтяжкой (для кнопки из прошлого задания)
    GPIOB->MODER &= ~(0x3U << (0 * 2U));
    GPIOB->PUPDR &= ~(0x3U << (0 * 2U));
    GPIOB->PUPDR |=  (0x1U << (0 * 2U));

    // ЗАДАНИЕ 5.2: Выбираем, на каком разряде будет гореть точка
    // 0 - крайний левый (первая цифра), 1 - вторая, 2 - третья, 3 - крайний правый
    int dot_pos = 1; // Например, пусть точка горит после второй цифры (20.26)

    while (1) {
        // Проверка кнопки (переключение года)
        if ((GPIOB->IDR & (1U << 0U)) == 0) {
            number[0] = 2; number[1] = 0; number[2] = 0; number[3] = 5;
        } else {
            number[0] = 2; number[1] = 0; number[2] = 2; number[3] = 6;
        }

        // Динамическая индикация
        for (int pos = 0; pos < 4; pos++) {
            GPIOC->BSRR = (0x000F << 16U); // Гасим все разряды
            
            // Получаем базовый код цифры
            uint8_t code = digit_code[number[pos]];
            
            // Если текущий разряд совпадает с позицией точки, 
            // добавляем точку (устанавливаем старший бит в 1)
            if (pos == dot_pos) {
                code |= 0x80; 
            }
            
            // Отправляем код с точкой (или без) на сегменты
            GPIOA->ODR = code; 
            
            GPIOC->BSRR = (1U << pos); // Включаем нужный разряд
            delay_scan(2000); // Задержка
        }
    }
}