/*
 * hal_adc.h
 *
 * Created: 09/12/2024 21:00:31
 *  Author: axieo
 */ 
#include <stdint.h>

#ifndef HAL_ADC_H_
#define HAL_ADC_H_

void configure_adc(void);
uint16_t read_luminosidade(void);
float read_luminosidade_per(uint16_t valor);



#endif /* HAL_ADC_H_ */