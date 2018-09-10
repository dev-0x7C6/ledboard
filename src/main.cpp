#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "animation/rainbow_animation.hpp"
#include "hal/regs.hpp"
#include "hal/ws2812b.hpp"

constexpr auto led_count = 768;

int main ()
{
	u8 data[led_count] = {};

	port<regs::ddr_d, 5>::hi();
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);
	rainbow_animation animation;
	
	for(;;) {
		for (u16 i = 0; i < sizeof(data)/3;)
		{
			static_assert(sizeof(data) % 3 == 0);
			data[i++] = animation.g();
			data[i++] = animation.r();
			data[i++] = animation.b();
		}
		
		ws.write<ws_type::ws2812b>(data, sizeof(data));
		
		animation.step();
		_delay_ms(100);
	}

	return 0;
}
