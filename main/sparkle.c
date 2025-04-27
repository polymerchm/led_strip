
/**
 *
 * sparkle.c
 *
 *  This is the main routine for programming the 20 elements NEOPIXEL strip to look like
 *  "fairy lights" or fireflies.
 *
 * Author: spollack
 * Date: 2025-04-09 19:05:20
 */

#include "sdkconfig.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "esp_check.h"
#include "driver/rmt_tx.h"
#include "esp_timer.h"

#include "led_strip_encoder.h"

#include "gaussian.h"
#include "ran0.h"
#include "color_print_utils.h"

/**
 * setup constants
 */

 // buffer => frame => led(3)
uint8_t buffer[2][CONFIG_SPARKLE_TIME_FRAMES][CONFIG_SPARKLE_NUM_PIXELS * 3] ;

long idum = 12311451; // seed for the ran0()

TaskHandle_t display_handle = NULL;
TaskHandle_t build_handle = NULL;

/**
 * globals for tasks
 */

rmt_channel_handle_t led_chan = NULL;
rmt_tx_channel_config_t tx_chan_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
    .gpio_num = CONFIG_SPARKLE_RMT_LED_STRIP_GPIO_NUM,
    .mem_block_symbols = 64, // increase the block size can make the LED less flickering
    .resolution_hz = CONFIG_SPARKLE_RMT_LED_STRIP_RESOLUTION_HZ,
    .trans_queue_depth = 4};
rmt_encoder_handle_t led_encoder = NULL;
led_strip_encoder_config_t encoder_config = {
    .resolution = CONFIG_SPARKLE_RMT_LED_STRIP_RESOLUTION_HZ};
rmt_transmit_config_t tx_config = {
    .loop_count = 0};


#define TAG "sparkle"

void start_buffer_build(uint8_t buffer_index)
{
    /**
     *  buffer_start: start of buffer, either block 1 or block 2
     *  density: fraction of pixels to populate with a pulse
     */
    memset(&buffer[buffer_index], 0, sizeof(buffer)/2);
 

    int num_to_pulse = random_in_range(2*CONFIG_SPARKLE_NUM_PIXELS, 5*CONFIG_SPARKLE_NUM_PIXELS, &idum);


    // range of time frames ( corrected for leadig or trailing edge of the pulses)
    uint16_t min_frame = (CONFIG_SPARKLE_PULSE_DURATION_FRAMES - 1) / 2;
    uint16_t max_frame = CONFIG_SPARKLE_TIME_FRAMES - min_frame - 1;


    uint8_t led_num;             // led to pulse
    uint16_t pulse_center_frame; // center of the pulse in the "time" reference frameframe
    uint8_t pulse_max;           // the maximum of the pulse
    float std_dev;               // the std-dev of the pulse;
    uint16_t pulse_index;         // pointer to the frame this peak's time corresponds to
    uint8_t std_dev_min = (int) CONFIG_SPARKLE_PULSE_DURATION_FRAMES / 10.0,
            std_dev_max = (int) CONFIG_SPARKLE_PULSE_DURATION_FRAMES / 6.0;
    uint8_t next_level;
    int offset;
    for (int i = 0; i < num_to_pulse; i++)
    {
        led_num = (uint8_t)random_in_range(CONFIG_SPARKLE_NUM_PIXELS - 1, 0, &idum);
        pulse_center_frame = random_in_range(max_frame, min_frame, &idum); 
        // this creates the time offset
        pulse_index = pulse_center_frame - min_frame - 1;
        pulse_max = random_in_range(255, 60, &idum); // between 255 and 60 (100% and 24% max intensity)
        std_dev = random_in_range(std_dev_max, std_dev_min, &idum);
        for (uint16_t frame_index = 0; frame_index < CONFIG_SPARKLE_PULSE_DURATION_FRAMES; frame_index++)
        {
            // this walks down the time steps (pulse index) and set 3 led values (led_num)
            next_level = (uint8_t)(gaussian((float)pulse_index,
                                            (float)pulse_center_frame, std_dev) *
                                   pulse_max);
            uint8_t *pixel = &buffer[buffer_index][pulse_index][led_num*3];
            uint8_t old_value = *pixel;
            if (old_value == 0)
            {
                memset(pixel, next_level, 3);
            }
            else
            {
                // if two are close in time, could cause a hicough, average
                uint8_t average =(next_level + old_value) / 2;
                memset(pixel, average, 3);
            }
            pulse_index++;
        }
    }                           
}

void display_next_buffer(uint8_t buffer_index) {
    // display this buffer
    // wait(bufferEmpty)
    for (uint16_t frame=0; frame < CONFIG_SPARKLE_TIME_FRAMES; frame++) {
        ESP_ERROR_CHECK(rmt_transmit(led_chan,
            led_encoder,
            &buffer[buffer_index][frame][0],
            CONFIG_SPARKLE_NUM_PIXELS*3,
            &tx_config));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(led_chan, portMAX_DELAY));
        vTaskDelay(CONFIG_SPARKLE_TIME_PER_FRAME/portTICK_PERIOD_MS);
    }
}


void display_buffers_task(void *pv) {
    uint32_t long_buffer_index;
    uint64_t start_time;
    uint64_t end_time;
    for(;;) {
            //wait here for a message to display a buffer

            if (xTaskNotifyWait(0,0, &long_buffer_index, portMAX_DELAY) == pdTRUE) {
                uint8_t buffer_index = (uint8_t) long_buffer_index;
                uint32_t next_index = (uint32_t) (buffer_index == 0 ? 1 : 0);
                // before displaying, tell the build to switch to the other buffer
                xTaskNotify(build_handle, next_index, eSetValueWithOverwrite);
                start_time = esp_timer_get_time();
                display_next_buffer(buffer_index);
                end_time = esp_timer_get_time(); 
            } else {
                PRINT_COLOR(A_MAGENTA, "xTaskNotifyWait timed out in display_buffer_task\n");
            }
           
        }   // tell build done with buffer

    
}

void build_next_buffer_task(void *pv) {
    uint32_t long_buffer_index;
    uint64_t start_time;
    uint64_t end_time;
    for(;;) {
        // wait for where to build next buffer
        if (xTaskNotifyWait(0,0, &long_buffer_index, portMAX_DELAY) == pdTRUE) {
            // build the new image
            start_time = esp_timer_get_time();
            start_buffer_build((uint8_t) long_buffer_index);
            end_time = esp_timer_get_time();
            xTaskNotify(display_handle, long_buffer_index, eSetValueWithOverwrite);
        } else {
            PRINT_COLOR(A_RED, "\t\txTaskNotifyWait timed out in build_next_buffer_task");
        }
    }

}

void app_main(void)
{
    /**
     * create the RMT_tx channel
     */

    ESP_LOGI(TAG, "Create RMT TX channel");
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    ESP_LOGI(TAG, "Install led strip encoder");
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(led_chan));

 
    xTaskCreate(
        build_next_buffer_task, 
        "build", 
        CONFIG_SPARKLE_BUILD_STACK_DEPTH, 
        NULL, 
        0,  
        &build_handle
        );

    xTaskCreate(
        display_buffers_task, 
        "display",
         CONFIG_SPARKLE_DISPLAY_STACK_DEPTH, 
         NULL,
         1, // priority
         &display_handle
        );

    xTaskNotify(build_handle, 0UL, eSetValueWithOverwrite); // initial build of the first buffer

}
