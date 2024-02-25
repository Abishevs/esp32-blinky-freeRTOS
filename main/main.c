#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
// #include "esp_log.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"

#define LED_ON_GPIO 15
#define LED_OFF_GPIO 2
#define SWITCH_BTN_GPIO 16

static uint8_t systemState = 0; // On or Off (def 0: off) 

static void configure_gpios(void) {
	// OFF led is set to 1, thus it will start blinking
	// On LED is set to 0
	gpio_reset_pin(LED_ON_GPIO);
	gpio_reset_pin(LED_OFF_GPIO);
	gpio_reset_pin(SWITCH_BTN_GPIO);

	gpio_set_direction(LED_ON_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED_OFF_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_direction(SWITCH_BTN_GPIO, GPIO_MODE_INPUT);

	gpio_pullup_en(SWITCH_BTN_GPIO); // enables interanal pullup resistor behavior
					 
	// gpio_set_level(LED_ON_GPIO, systemState);
	// gpio_set_level(LED_OFF_GPIO, !systemState);

}

// static void blink_led(gpio_num_t gpio_num, uint8_t level) {
// 	gpio_set_level(gpio_num, level);
// }

void btn_task(void* arg) {
	uint8_t currentBtnState;
	uint8_t lastBtnState = 1;
	
	while (1) {
		currentBtnState = gpio_get_level(SWITCH_BTN_GPIO);
		if (currentBtnState == 0 && lastBtnState != 0) {
			vTaskDelay(pdMS_TO_TICKS(50)); // Debounc delay
			if (gpio_get_level(SWITCH_BTN_GPIO) == 0 ) {
				systemState = !systemState;
			}

		}
		lastBtnState = currentBtnState;
		vTaskDelay(pdMS_TO_TICKS(10)); // delay for state polling

	}
}

void led_task(void* arg) {
	while (1) {
		if (systemState == 0) { 
			// System Off:
			// Red button blinking.
			// Green BTN Off.
			gpio_set_level(LED_ON_GPIO, 0); //Turn Green LED Off
			gpio_set_level(LED_OFF_GPIO, 1); //Blink Red LED 
			vTaskDelay(pdMS_TO_TICKS(500)); 
			gpio_set_level(LED_OFF_GPIO, 0); 
			vTaskDelay(pdMS_TO_TICKS(500)); 

		} else { 
			// system On: 
			// Blink green LED. 
			// Red LED staticly On.
			gpio_set_level(LED_OFF_GPIO, 1); //Turn Red LED ON
			gpio_set_level(LED_ON_GPIO, 1); //Blink Green LED
			vTaskDelay(pdMS_TO_TICKS(500)); 
			gpio_set_level(LED_ON_GPIO, 0); 
			vTaskDelay(pdMS_TO_TICKS(500)); 
			
		}
	
	}
}

void app_main(void) {
	configure_gpios();

	xTaskCreate(
		btn_task,                 // Task Function
		"System State Switch",   // Task name (for debbuging)
		2048, 			// Stack size (in words)
		NULL,                  // Task input args
		1,                    // Prioritity
		NULL                 // Task Handle
	);

	xTaskCreate(
		led_task,                 // Task Function
		"Led State Switcher",    // Task name (for debbuging)
		2048, 			// Stack size (in words)
		NULL,                  // Task input args
		1,                    // Prioritity
		NULL                 // Task Handle
	);

}
