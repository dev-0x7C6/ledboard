#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <animation/animation-runners.hpp>
#include <animation/animation-modifiers.hpp>
#include <animation/generators/rainbow-animation.hpp>

#include <hal/regs.hpp>
#include <hal/ws2812b.hpp>
#include <pwm.hpp>

constexpr auto led_count = 38;

color_array<led_count * 3> data{};

void animation_loop(rgb value) {
	transformation<led_count>::rotate(reinterpret_cast<rgb *>(data.data()), value);
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);
	ws.write(data);
	_delay_ms(10);
}

int main() {
	port<regs::ddr_d, 5>::hi();
	for (;;) {
		sequential_animation<animation_loop,
			repeat<speed<to_grb888<rainbow_animation>, 8>, 8>,
			repeat<speed<to_grb888<rainbow_animation>, 4>, 4>,
			repeat<speed<to_grb888<rainbow_animation>, 2>, 2>>();
	}
	return 0;
}
