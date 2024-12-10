#include "asf.h"
#include "hal.h"

int main(void)
{
	init_board();
	
	while (1) {
		
		uint16_t luz = read_luminosidade();
		printf("Luz: %d\n", luz);
		float luz_per = read_luminosidade_per(read_luminosidade());
		printf("Porcentagem de luz: %.2f\n", luz_per);
		
		if (luz_per < 50) {
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			} else {
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}

	return 0;
}
