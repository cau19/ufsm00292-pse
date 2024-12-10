#include "asf.h"
#include "hal_adc.h"
#include "main.h"
#include "hal_temt6000.h"


void configure_adc(void)
{
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	config_adc.gain_factor     = ADC_GAIN_FACTOR_DIV2;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV32;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN8;
	config_adc.resolution      = ADC_RESOLUTION_12BIT;
	config_adc.freerunning     = true;
	config_adc.left_adjust     = false;
	adc_init(&temt6000_adc_instance, ADC, &config_adc);
	adc_enable(&temt6000_adc_instance);
}
