
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
#include "gaussian.h"

/**
 * setup constants
 */

uint8_t buffer[2 * 20 * 3 * CONFIG_TIME_FRAMES];
//                  dual buffer, 20 pixels , RGB, time interfals
uint8_t *next_buffer_p = NULL; // pointer to next buffer

uint16_t peak_time_of_centers[CONFIG_NUM_PIXELS]; 
// centroid of the times for each pixel,  -1 means its not pulsing

double pulse_profile[CONFIG_PULSE_DURATION_FRAMES];

#define TAG "sparkle"

void buffer_complete_cb(void) {
    /**
     *  buffer is ready, looping and transmit next buffer
     */

}

void generate_pulse_profile(void) {
    // assume 1/2 the width is 3 standard deviations

    double std_dev = CONFIG_PULSE_DURATION_FRAMES / 6.0;
    for (int i=0; i < CONFIG_PULSE_DURATION_FRAMES; i++) {
        double x = i - CONFIG_PULSE_DURATION_FRAMES/2;
        double y = gaussian(x, 0.0, std_dev);
        pulse_profile[i] = y;
        printf("X=%f  Y=%f\n",x,pulse_profile[i]);
    }
}

void start_buffer_build(uint8_t *buffer_start) {
    /**
     *  buffer_start: start of buffer, either block 1 or block 2
     *  density: fraction of pixels to populate with a pulse
     */
    memset(buffer_start, 0, 3*CONFIG_NUM_PIXELS*CONFIG_TIME_FRAMES); 
    memset(peak_time_of_centers, -1, CONFIG_NUM_PIXELS);
    int num_to_pulse =  rand() % (CONFIG_NUM_PIXELS/2-1) + 1;
    while (num_to_pulse > 0) {
        

        


    }


}


void display_next_buffer(uint8_t* buffer_start,  rmt_channel_handle_t led_chan,
    rmt_encoder_handle_t led_encoder, rmt_transmit_config_t *tx_config) {
    // display this buffer
    uint8_t* frame_p = 0;
    for (int i=0; i < CONFIG_TIME_FRAMES; i++) {
        frame_p = buffer_start + i*CONFIG_NUM_PIXELS*3;
        ESP_ERROR_CHECK(rmt_transmit(led_chan, 
            led_encoder, 
            frame_p, 
            CONFIG_NUM_PIXELS*3, 
            tx_config));  
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(led_chan, portMAX_DELAY));
        vTaskDelay(pdMS_TO_TICKS(CONFIG_TIME_PER_PULSE));
    }
}

void app_main(void) {
    generate_pulse_profile();
}
// {
//     /**
//      * create the RMT_tx channel
//      */
//     uint32_t red = 0;
//     uint32_t green = 0;
//     uint32_t blue = 0;
//     uint16_t hue = 0;
//     uint16_t start_rgb = 0;

//     ESP_LOGI(TAG, "Create RMT TX channel");
//     rmt_channel_handle_t led_chan = NULL;
//     rmt_tx_channel_config_t tx_chan_config = {
//         .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
//         .gpio_num = CONFIG_RMT_LED_STRIP_GPIO_NUM,
//         .mem_block_symbols = 64, // increase the block size can make the LED less flickering
//         .resolution_hz = CONFIG_RMT_LED_STRIP_RESOLUTION_HZ,
//         .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
//     };
//     ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

//     ESP_LOGI(TAG, "Install led strip encoder");
//     rmt_encoder_handle_t led_encoder = NULL;
//     led_strip_encoder_config_t encoder_config = {
//         .resolution = CONFIG_RMT_LED_STRIP_RESOLUTION_HZ,
//     };
//     ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

//     ESP_LOGI(TAG, "Enable RMT TX channel");
//     ESP_ERROR_CHECK(rmt_enable(led_chan));
//     ESP_LOGI(TAG, "First Next Buffer");
//     next_buffer_p = &buffer;
//     start_bufferBuild(next_buffer_p, &buffer_complete_cb);

//     rmt_tx_channel_config_t tx_config = {

//     }

//     display_next_buffer(next_buffer_p, led_chan, led_encoder, &tx_config);



    /**
     * create the build new page task
     */
// }
