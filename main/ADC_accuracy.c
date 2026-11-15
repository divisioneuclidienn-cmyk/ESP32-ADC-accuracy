#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include <string.h>
#include <math.h>

// c'est plutôt la bandwidth
#define ADC_BANDWITDH 4096
#define CURRENT_ADC_MAX 4095
#define CURRENT_ADC_MIN 0
#define MEASURES_NB 10000

const gpio_num_t input = GPIO_NUM_36; //ADC1_CHANNEL0 "SP" pin
const char * TAG = "ADC";

// 8kB array (4096 elements)
uint16_t raw_ADC_frequency[ADC_BANDWITDH] = {0};

typedef struct {
  float average;
  int ADCpp;
  float variance;
  float standard_deviation;
}ADC_results_t;
ADC_results_t g_ADC_results;

void sample_data(const uint16_t * data, ADC_results_t * ADC_results){

  uint64_t sum = 0;
  int min = CURRENT_ADC_MAX;
  int max = CURRENT_ADC_MIN;

  for(int ADC_value = 0; ADC_value < ADC_BANDWITDH; ADC_value++){
    // Calcul de la moyenne
    sum += ADC_value * data[ADC_value];

    // Calcul de l'amplitude
    if(data[ADC_value] >= 1 && ADC_value > max) max = ADC_value;
    if(data[ADC_value] >= 1 && ADC_value < min) min = ADC_value;
  }
  float average = (float)sum / MEASURES_NB;
  int ADCpp = max - min;
  ESP_LOGI("ADC_min/max", "min = %d, max = %d\n", min, max);


  // Calcul de la variance
  float sum_variance = 0;
  for(int ADC_value = 0; ADC_value < ADC_BANDWITDH; ADC_value++){
    // Si au moins 1 effectif est présent
    if(data[ADC_value] >= 1){
      float x_powered = (ADC_value - average) * (ADC_value - average); // float car average est un float
      sum_variance += data[ADC_value] * x_powered;
    }
  }
  float variance = (float)sum_variance / MEASURES_NB;
  float standard_deviation = sqrt(variance);

  // envoi des données
  ADC_results->average = average;
  ADC_results->ADCpp = ADCpp;
  ADC_results->variance = variance;
  ADC_results->standard_deviation = standard_deviation;
}

void dump_ADC_results(const ADC_results_t * s){
  ESP_LOGI(TAG, "Nb of lectures     = %d\n"
                "Average            = %.2f\n"
                "ADCpp              = %d\n"
                "Variance           = %.2f\n"
                "standard_deviation = %.2f\n", 
                MEASURES_NB, 
                s->average, 
                s->ADCpp, 
                s->variance, 
                s->standard_deviation);
}

void app_main(void){

  // setup de l'ADCx
  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1, // ADC1
    .ulp_mode = ADC_ULP_MODE_DISABLE // UltraLowPower mode
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
  
  // Setup de l'ADCx_y (channel de l'ADC)
  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_12,
    .atten = ADC_ATTEN_DB_12 // divise la tension d'entrée reçu car l'ADC fonctionne en 1.1-1.2V
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &config));

  while(1){

    int raw_ADC;
    for(uint64_t i = 0; i < MEASURES_NB; i++){
      ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_5, &raw_ADC));
      // Mise à jour du tableau de fréquence
      raw_ADC_frequency[raw_ADC] += 1;

      esp_rom_delay_us(100); // Délai régulier car sinon perturbations électriques trop élevées 
    }

    sample_data(&raw_ADC_frequency[0], &g_ADC_results);
    dump_ADC_results(&g_ADC_results);
    memset(&raw_ADC_frequency[0], 0, sizeof(raw_ADC_frequency)); // remise à 0 de tous les éléments du tableau
    esp_rom_delay_us(1000000);
  }

}

// On se base sur le pinout de l'esp pour trouver les broches intégrants des ADC
// On remarque 2 groupes : ADC1_x et ADC2_x
// On remarque que l'ADC2 ne peut pas être uitilisé quand le WiFi est activé (Il l'utilise pour fonctionner)
// fonctionnement ADC intéressant (google)
// SAR = Successive Approximation Register = décrit le fonctionnement de l'ADC
// lecture par ADC bloquante car charge des condensateurs d'échantillonnages + N cycles d'horloges en fonctions de la résolution de l'ADC
// mode DMA pour du non bloquant

// calibrage de la tension avec adc_cali
// PRIu32

// OBJECTIF : Faire une courbe de linéarité de l'ADC de MON ESP
// courbe de distribution du bruit possible grâce aux calculs de l'écart type
// Calcul de la variance puis de l'écart type : https://www.youtube.com/watch?v=YcTgP2d_HI8
// -> Tableau : Valeurs/Effectifs
// Algo de welford pour éviter les gros tableaux comme ça (math trop complexes)