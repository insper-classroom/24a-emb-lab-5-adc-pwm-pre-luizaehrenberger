#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

#define WINDOW_SIZE 5

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
    int buffer[WINDOW_SIZE] = {0};
    int bufferIndex = 0;
    int total = 0; // Vamos usar total ao invés de sum para evitar confusão com a função sum()
    int count = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, portMAX_DELAY)) {
            // Remove o valor mais antigo do total
            total -= buffer[bufferIndex];

            // Adiciona o novo valor ao buffer e ao total
            buffer[bufferIndex] = data;
            total += data;

            // Atualiza o índice para o próximo valor do buffer
            bufferIndex = (bufferIndex + 1) % WINDOW_SIZE;

            // Aumenta o contador até atingir o tamanho da janela
            if (count < WINDOW_SIZE) {
                count++;
            }

            // Calcula a média móvel
            int average = total / count;

            // Imprime o valor médio
            printf("Filtered Value: %d\n", average);

            // Manter o delay conforme especificado
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
