#pragma once

#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#define ADC_BITWIDTH (4096)
#define CURRENT_ADC_MAX (4095)
#define CURRENT_ADC_MIN (0)
#define MEASURES_NB (1024)

#define ADC_UNIT          ADC_UNIT_1
#define ADC_ULP_MODE      ADC_ULP_MODE_DISABLE
#define ADC_BITWIDTH      ADC_BITWIDTH_12
#define ADC_ATTEN         ADC_ATTEN_DB_12

typedef struct {
  float average;
  int ADCpp;
  float variance;
  float standard_deviation;
  int calibrated_voltage;
}ADC_results_t;

void init_ADC(adc_oneshot_unit_handle_t * adc_handle, adc_cali_handle_t * calibration_handle);
void sample_data(const uint16_t * data, ADC_results_t * ADC_results, adc_cali_handle_t calibration_handle);
void dump_ADC_results_csv(uint64_t time_us, int raw_adc, float filtered_value, const ADC_results_t * s);
void dump_ADC_results_monitor(const ADC_results_t * s);
void print_CSV_headers(const char * headers);
void start_CSV();
void end_CSV();