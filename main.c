#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BTN_ABRIR     GPIO_NUM_4
#define BTN_CERRAR    GPIO_NUM_5
#define SENSOR        GPIO_NUM_18

#define MOTOR_ABRIR   GPIO_NUM_19
#define MOTOR_CERRAR  GPIO_NUM_21

int estado_porton = 0; // 0=cerrado, 1=abierto, 2=movimiento

void app_main(void) {

    gpio_set_direction(BTN_ABRIR, GPIO_MODE_INPUT);
    gpio_set_direction(BTN_CERRAR, GPIO_MODE_INPUT);
    gpio_set_direction(SENSOR, GPIO_MODE_INPUT);

    gpio_set_direction(MOTOR_ABRIR, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_CERRAR, GPIO_MODE_OUTPUT);

    while (1) {

        int abrir = gpio_get_level(BTN_ABRIR);
        int cerrar = gpio_get_level(BTN_CERRAR);
        int sensor = gpio_get_level(SENSOR);

        // ABRIR
        if (abrir && estado_porton != 1) {
            gpio_set_level(MOTOR_ABRIR, 1);
            gpio_set_level(MOTOR_CERRAR, 0);

            estado_porton = 2;
            vTaskDelay(pdMS_TO_TICKS(5000));

            gpio_set_level(MOTOR_ABRIR, 0);
            estado_porton = 1;
        }

        // CERRAR
        if (cerrar && estado_porton != 0) {
            gpio_set_level(MOTOR_ABRIR, 0);
            gpio_set_level(MOTOR_CERRAR, 1);

            estado_porton = 2;

            for (int i = 0; i < 50; i++) {

                if (gpio_get_level(SENSOR)) {
                    gpio_set_level(MOTOR_CERRAR, 0);
                    estado_porton = 1;
                    break;
                }

                vTaskDelay(pdMS_TO_TICKS(100));
            }

            gpio_set_level(MOTOR_CERRAR, 0);
            estado_porton = 0;
        }

        // CIERRE AUTOMÁTICO
        if (estado_porton == 1) {

            vTaskDelay(pdMS_TO_TICKS(10000));

            if (!gpio_get_level(SENSOR)) {
                gpio_set_level(MOTOR_CERRAR, 1);

                vTaskDelay(pdMS_TO_TICKS(5000));

                gpio_set_level(MOTOR_CERRAR, 0);
                estado_porton = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}