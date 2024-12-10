#include "hal.h"
#include "asf.h"

void init_board(void)
{
	system_init();
	configure_console();
	configure_adc();
}
