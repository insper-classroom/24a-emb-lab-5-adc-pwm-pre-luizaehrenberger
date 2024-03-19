#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/adc.h"

void adc_1_task(void *p) {
    adc_init();
    adc_gpio_init(27); // Inicializa GPIO 27 para ADC
    adc_gpio_init(26); // Inicializa GPIO 26 para ADC

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);

    uint16_t result1, result2;
    while (1) {
        // Leitura ADC 1 (GP27)
        adc_select_input(1); // Seleciona entrada ADC 1
        result1 = adc_read();
        printf("voltage 1: %f V\n", result1 * conversion_factor);

        // Leitura ADC 0 (GP26)
        adc_select_input(0); // Seleciona entrada ADC 0
        result2 = adc_read();
        printf("voltage 2: %f V\n", result2 * conversion_factor);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");
    adc_init();

    xTaskCreate(adc_1_task, "ADC_Task", 4095, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {
    }
}
