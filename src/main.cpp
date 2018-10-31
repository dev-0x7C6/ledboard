#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <animation/animation-runners.hpp>
#include <animation/animation-modifiers.hpp>
#include <animation/generators/rainbow-animation.hpp>
#include <animation/generators/plain-color-animation.hpp>

#include <hal/regs.hpp>
#include <hal/ws2812b.hpp>
#include <pwm.hpp>

constexpr auto led_count = 38;

color_array<led_count * 3> data{};

void initialize() {
	port<regs::ddr_d, 5>::hi();
}

void animation_loop(rgb value) {
	transformation<led_count>::rotate(reinterpret_cast<rgb *>(data.data()), value);
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);
	ws.write(data);
	_delay_ms(20);
}

int main() {
	initialize();

	for (;;) {
		sequential_animation<animation_loop, to_grb888,
			repeat<speed<plain_color_animation<0x00, 0x00, 0xff>, 8>, 20>,
			repeat<speed<plain_color_animation<0xff, 0x20, 0x00>, 8>, 10>,
			repeat<speed<plain_color_animation<0x00, 0x00, 0xff>, 8>, 20>,
			repeat<speed<plain_color_animation<0x00, 0xff, 0x00>, 8>, 10>,
			repeat<speed<rainbow_animation, 8>, 8>,
			repeat<speed<rainbow_animation, 4>, 4>,
			repeat<speed<rainbow_animation, 2>, 2>>();
	}

	return 0;
}
