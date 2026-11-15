#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_BITWIDTH 4096
#define CURRENT_ADC_MAX 4095
#define CURRENT_ADC_MIN 0
#define MEASURES_NB 10000

const gpio_num_t input = GPIO_NUM_36; //ADC1_CHANNEL0 "SP" pin
const char * TAG = "ADC";

// 8kB array (4096 elements)
uint16_t raw_ADC_frequency[ADC_BITWIDTH] = {0};

adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t calibration_handle = NULL;

typedef struct {
  float average;
  int ADCpp;
  float variance;
  float standard_deviation;
  int calibrated_voltage;
}ADC_results_t;
ADC_results_t g_ADC_results;

void sample_data(const uint16_t * data, ADC_results_t * ADC_results){

  uint64_t sum = 0;
  int min = CURRENT_ADC_MAX;
  int max = CURRENT_ADC_MIN;

  for(int ADC_value = 0; ADC_value < ADC_BITWIDTH; ADC_value++){
    // Moyenne
    sum += ADC_value * data[ADC_value];

    // Amplitude
    if(data[ADC_value] >= 1 && ADC_value > max) max = ADC_value;
    if(data[ADC_value] >= 1 && ADC_value < min) min = ADC_value;
  }
  float average = (float)sum / MEASURES_NB;
  int ADCpp = max - min;
  ESP_LOGI("ADC_min/max", "min = %d, max = %d\n", min, max);


  // Variance
  float sum_variance = 0;
  for(int ADC_value = 0; ADC_value < ADC_BITWIDTH; ADC_value++){
    
    // Si au moins 1 effectif est présent
    if(data[ADC_value] >= 1){
      float x_powered = (ADC_value - average) * (ADC_value - average);
      sum_variance += data[ADC_value] * x_powered;
    }
  }
  float variance = (float)sum_variance / MEASURES_NB;
  float standard_deviation = sqrt(variance);

  int calibrated_voltage;
  ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibration_handle, (int)average, &calibrated_voltage));


  ADC_results->average = average;
  ADC_results->ADCpp = ADCpp;
  ADC_results->variance = variance;
  ADC_results->standard_deviation = standard_deviation;
  ADC_results->calibrated_voltage = calibrated_voltage;
}

void dump_ADC_results(const ADC_results_t * s){
  ESP_LOGI(TAG, "Nb of lectures     = %d\n"
                "Average            = %.2f\n"
                "ADCpp              = %d\n"
                "Variance           = %.2f\n"
                "standard_deviation = %.2f\n"
                "calibrated_voltage = %d\n", 
                MEASURES_NB, 
                s->average, 
                s->ADCpp, 
                s->variance, 
                s->standard_deviation,
                s->calibrated_voltage);
}

void app_main(void){

  // setup de l'ADCx
  adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1, // ADC1
    .ulp_mode = ADC_ULP_MODE_DISABLE // UltraLowPower mode
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
  
  // Setup de l'ADCx_y (channel de l'ADC)
  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_12,
    .atten = ADC_ATTEN_DB_12
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));


  adc_cali_line_fitting_config_t calibration_config = {
    .unit_id = ADC_UNIT_1,
    .bitwidth = ADC_BITWIDTH_12,
    .atten = ADC_ATTEN_DB_12
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&calibration_config, &calibration_handle));


  while(1){

    int raw_ADC;
    for(uint64_t i = 0; i < MEASURES_NB; i++){
      ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &raw_ADC));
      // Mise à jour du tableau de fréquence
      raw_ADC_frequency[raw_ADC] += 1;

      esp_rom_delay_us(80); // Délai régulier car sinon perturbations électriques trop élevées (ADCpp uniquement)
    }

    sample_data(&raw_ADC_frequency[0], &g_ADC_results);
    dump_ADC_results(&g_ADC_results);
    memset(&raw_ADC_frequency[0], 0, sizeof(raw_ADC_frequency)); // remise à 0 de tous les éléments du tableau
    vTaskDelay(pdMS_TO_TICKS(100));
  }

}