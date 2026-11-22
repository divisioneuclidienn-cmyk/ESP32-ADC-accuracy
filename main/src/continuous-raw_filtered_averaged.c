#include <stdio.h>
#include "esp_rom_sys.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/ADC.h"
#include "esp_timer.h"

// 8kB array (4096 elements)
uint16_t raw_ADC_frequency[ADC_BITWIDTH] = {0};
uint16_t filtered_ADC_frequency[ADC_BITWIDTH] = {0};
float filtered_value = 0;
float alpha = 0.1;

bool first_sample = true;

adc_oneshot_unit_handle_t adc_handle;
adc_cali_handle_t calibration_handle = NULL;

ADC_results_t ADC_results;

const char * headers = "time_us;raw_ADC;filtered_ADC;measures_nb;average;ADCpp;variance;standard_deviation;calibrated_voltage";

void app_main(void){

  init_ADC(&adc_handle, &calibration_handle);

  int raw_ADC;
  uint64_t time_us = 0;
  start_CSV();
  print_CSV_headers(headers);
  while(1){

    for(uint64_t i = 0; i < MEASURES_NB; i++){
      ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &raw_ADC));
      time_us = esp_timer_get_time();

      if(first_sample){
        first_sample = false;
        filtered_value = raw_ADC;
      }

      // Mise à jour du tableau de fréquence
      raw_ADC_frequency[raw_ADC] += 1;

      filtered_value = alpha * raw_ADC + (1 - alpha) * filtered_value;
      
      if(i == MEASURES_NB - 1){
        sample_data(&raw_ADC_frequency[0], &ADC_results, calibration_handle);
        dump_ADC_results_csv(time_us, raw_ADC, filtered_value, &ADC_results);
      }
      else{
        dump_ADC_results_csv(time_us, raw_ADC, filtered_value, NULL);
      }

      esp_rom_delay_us(1000); //1ms
    }

    memset(&raw_ADC_frequency[0], 0, sizeof(raw_ADC_frequency)); // remise à 0 de tous les éléments du tableau
    vTaskDelay(1);
  }
}