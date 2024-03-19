#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int movingAverageBuffer[5] = {0}; // pq é 5
    int bufferIndex = 0;
    int sum = 0;
    int count = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, portMAX_DELAY)) {
            // Subtrai o valor antigo e adiciona o novo valor à soma
            sum -= movingAverageBuffer[bufferIndex];
            sum += data;

            // Armazena o novo valor no buffer
            movingAverageBuffer[bufferIndex] = data;
            bufferIndex = (bufferIndex + 1) % 5;

            // Incrementa o contador até atingir o tamanho da janela
            count = (count < 5) ? count + 1 : 5;

            // Calcula a média móvel
            int average = (count > 0) ? (sum / count) : 0;

            // Imprime o valor médio
            printf("Filtered Value: %d\n", average);

            // Deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
