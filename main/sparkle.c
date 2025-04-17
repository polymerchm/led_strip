
/**
 *
 * sparkle.c
 *
 *  This is the main routine for programming the 20 elements NEOPIXEL strip to look like
 *  "fairy lights"
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
#include "esp_log.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "esp_check.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
#include "gaussian.h"
#include "ran0.h"

/**
 * setup constants
 */

uint8_t buffer[2 * CONFIG_SPARKLE_NUM_PIXELS * 3 * CONFIG_SPARKLE_TIME_FRAMES];
//                  dual buffer, 20 pixels , RGB, time interfals
uint8_t *next_buffer_p = buffer; // pointer to next

long idum = 12311451; // seed for the ran0()

#define TAG "sparkle"

// void buffer_complete_cb(void) {
//     /**
//      *  buffer is ready, looping and transmit next buffer
//      */

// }

void start_buffer_build(uint8_t *buffer_start)
{

    /**
     *  buffer_start: start of buffer, either block 1 or block 2
     *  density: fraction of pixels to populate with a pulse
     */
    memset(buffer_start, 0, 3 * CONFIG_SPARKLE_NUM_PIXELS * CONFIG_SPARKLE_TIME_FRAMES);
    // for (int i=0; i < CONFIG_SPARKLE_NUM_PIXELS; i++) {
    //     memset(peaks[i].values, 0, CONFIG_SPARKLE_PULSE_DURATION_FRAMES);
    // }

    int num_to_pulse = random_in_range(CONFIG_SPARKLE_NUM_PIXELS * 2 / 3, 1, &idum);

    // range of time frames ( corrected for leadig or trailing edge of the pulses)
    uint8_t min_frame = (CONFIG_SPARKLE_PULSE_DURATION_FRAMES - 1) / 2;
    uint8_t max_frame = CONFIG_SPARKLE_TIME_FRAMES - min_frame - 1;

    uint8_t led_num;             // led to pulse
    uint16_t pulse_center_frame; // center of the pulse in the "time" reference frameframe
    uint8_t pulse_max;           // the maximum of the pulse
    float std_dev;               // the std-dev of the pulse;
    uint8_t pulse_index;         // pointer to the frame this peak's time corresponds to
    uint8_t std_dev_min = CONFIG_SPARKLE_PULSE_DURATION_FRAMES / 12,
            std_dev_max = CONFIG_SPARKLE_PULSE_DURATION_FRAMES / 6;
    uint8_t next_level;
    int offset;

    for (int i = 0; i < num_to_pulse; i++)
    {
        led_num = (uint8_t)random_in_range(CONFIG_SPARKLE_NUM_PIXELS - 1, 0, &idum);
        pulse_center_frame = random_in_range(max_frame, min_frame, &idum); // this creates the time offset
        pulse_index = pulse_center_frame - min_frame - 1;

        pulse_max = random_in_range(255, 60, &idum); // between 255 and 60 (100% and 24% max intensity)
        std_dev = random_in_range(std_dev_max, std_dev_min, &idum);

        for (int frame_index = 0; frame_index < CONFIG_SPARKLE_PULSE_DURATION_FRAMES; frame_index++)
        {
            // this walks down the time steps (pulse index) and set 3 led values (led_num)
            offset = (pulse_index * CONFIG_SPARKLE_NUM_PIXELS + led_num) * 3;
            next_level = (uint8_t)(gaussian((float)pulse_index,
                                            (float)pulse_center_frame, std_dev) *
                                   pulse_max);
            uint8_t old_value = *(buffer_start + offset);
            if (old_value == 0)
            {
                memset(buffer_start + offset, next_level, 3);
            }
            else
            {
                // if two are close in time, could cause a hicough, average
                memset(buffer_start + offset, (next_level + old_value) / 2, 3);
                pulse_index++; // next position along the gaussian pulse profile (in time)
            }
        }
    }
}

void display_next_buffer(uint8_t* buffer_start,  rmt_channel_handle_t led_chan, rmt_encoder_handle_t led_encoder, rmt_transmit_config_t *tx_config) {
    // display this buffer
    uint8_t* frame_p = 0;
    for (int i=0; i < CONFIG_SPARKLE_TIME_FRAMES; i++) {
        frame_p = buffer_start + i*CONFIG_SPARKLE_NUM_PIXELS*3;
        ESP_ERROR_CHECK(rmt_transmit(led_chan,
            led_encoder,
            frame_p,
            CONFIG_SPARKLE_NUM_PIXELS*3,
            tx_config));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(led_chan, portMAX_DELAY));
        vTaskDelay(pdMS_TO_TICKS(CONFIG_SPARKLE_TIME_PER_FRAME));
    }
}

// void buffer_complete_cb() {

// }

void app_main(void)
{

    /**
     * create the RMT_tx channel
     */

    ESP_LOGI(TAG, "Create RMT TX channel");
    rmt_channel_handle_t led_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = CONFIG_SPARKLE_RMT_LED_STRIP_GPIO_NUM,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = CONFIG_SPARKLE_RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    ESP_LOGI(TAG, "Install led strip encoder");
    rmt_encoder_handle_t led_encoder = NULL;
    led_strip_encoder_config_t encoder_config = {
        .resolution = CONFIG_SPARKLE_RMT_LED_STRIP_RESOLUTION_HZ};
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(led_chan));
    ESP_LOGI(TAG, "First Next Buffer");
    next_buffer_p = buffer;
    // start_buffer_build(next_buffer_p);

    // rmt_tx_channel_config_t tx_config = {

    // };

    // // this will be a task
    // display_next_buffer(next_buffer_p, led_chan, led_encoder, &tx_config);
    // // this too
    // build_next_buffer(&next_buffer_p, sizeof(buffer));

    /**
     * create the build new page task
     */
}
