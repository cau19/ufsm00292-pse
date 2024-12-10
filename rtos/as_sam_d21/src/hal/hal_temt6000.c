#include "asf.h"
#include "hal_adc.h"
#include "main.h"


uint16_t read_luminosidade(void)
{
	uint16_t luz_val;
	status_code_genare_t adc_status;
	adc_start_conversion(&temt6000_adc_instance);
	do {
		adc_status = adc_read(&temt6000_adc_instance, &luz_val);
	} while (adc_status == STATUS_BUSY);
	return LIGHT_SENSOR_RESOLUTION - luz_val;
}

float read_luminosidade_per(uint16_t valor)
{	float valor_per= ((float)valor/(float)LIGHT_SENSOR_RESOLUTION)*100.0f;
	return valor_per;
}