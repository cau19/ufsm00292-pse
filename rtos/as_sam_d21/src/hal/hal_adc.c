#include "asf.h"
#include "hal_adc.h"
#include "main.h"
/* Light sensor module. */
struct adc_module adc_instance;

 uint16_t read_luminosidade(void)
{
	uint16_t luz_val;
	status_code_genare_t adc_status;
	adc_start_conversion(&adc_instance);
	do {
		adc_status = adc_read(&adc_instance, &luz_val);
	} while (adc_status == STATUS_BUSY);
	return LIGHT_SENSOR_RESOLUTION - luz_val;
}

float read_luminosidade_per(uint16_t valor)
{	float valor_per= ((float)valor/(float)LIGHT_SENSOR_RESOLUTION)*100.0f;
	return valor_per;
}

void configure_adc(void)
{
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	config_adc.gain_factor     = ADC_GAIN_FACTOR_DIV2;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV32;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN18;
	config_adc.resolution      = ADC_RESOLUTION_12BIT;
	config_adc.freerunning     = true;
	config_adc.left_adjust     = false;
	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
}
