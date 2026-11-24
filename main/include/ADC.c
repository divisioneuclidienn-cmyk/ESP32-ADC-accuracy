#include "ADC.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include <math.h>

void init_ADC(adc_oneshot_unit_handle_t * adc_handle, adc_cali_handle_t * calibration_handle){
  // setup de l'ADCx
  adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT, // ADC1
    .ulp_mode = ADC_ULP_MODE // UltraLowPower mode
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc_handle));
  
  // Setup de l'ADCx_y (channel de l'ADC)
  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH,
    .atten = ADC_ATTEN
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc_handle, ADC_CHANNEL_0, &config));


  adc_cali_line_fitting_config_t calibration_config = {
    .unit_id = ADC_UNIT,
    .bitwidth = ADC_BITWIDTH,
    .atten = ADC_ATTEN
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&calibration_config, calibration_handle));
}


void sample_data(const uint16_t * data, ADC_results_t * ADC_results, adc_cali_handle_t calibration_handle){

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

void dump_ADC_results_csv(uint64_t time_us, int raw_adc, float filtered_value, const ADC_results_t * s){
  
  if(s != NULL){
    printf(     "%llu;"
                "%d;"
                "%f;"
                "%d;"
                "%.2f;"
                "%d;"
                "%.2f;"
                "%.2f;"
                "%d\n",
                time_us,
                raw_adc,
                filtered_value,
                MEASURES_NB, 
                s->average, 
                s->ADCpp, 
                s->variance, 
                s->standard_deviation,
                s->calibrated_voltage);
  }
  else{
    printf("%llu;%d;%f;;;;;;\n", time_us, raw_adc, filtered_value);
  }
  
}

void start_CSV(){
  printf("CSV_START\n");
}

void end_CSV(){
  printf("CSV_END\n");
}

void print_CSV_headers(const char * headers){
  printf("%s\n", headers);
}

void dump_ADC_results_monitor(const ADC_results_t * s){
  ESP_LOGI("ADC", "Nb of lectures     = %d\n"
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