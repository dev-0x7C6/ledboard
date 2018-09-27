#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <animation/rainbow_animation.hpp>
#include <hal/regs.hpp>
#include <hal/ws2812b.hpp>
#include <pwm.hpp>

#include <etl/array.h>

constexpr auto led_count = 38;

int main() {
	color_array<led_count * 3> data{};

	port<regs::ddr_d, 5>::hi();
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);

	animator<rainbow_animation, ws.palette(), led_count> animator(reinterpret_cast<rgb *>(data.data()));

	for (;;) {
		animator.rotate(5);
		ws.write(data);
		_delay_ms(20);
	}

	return 0;
}
