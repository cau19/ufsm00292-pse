#include <stdint.h>

#ifndef TEMT6000_H_
#define TEMT6000_H_
#define LIGHT_SENSOR_RESOLUTION	      4096    /* ADC_RESOLUTION_12BIT */
uint16_t read_luminosidade(void);
float read_luminosidade_per(uint16_t valor);
/* Light sensor module. */
struct adc_module temt6000_adc_instance;


#endif /* TEMT6000_H_ */