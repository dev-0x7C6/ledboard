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

constexpr auto led_count = 100;

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
	_delay_ms(30);
}

constexpr void forwarding_light_strips() {
	constexpr auto long_bar = static_cast<int>(led_count * 1.00);
	constexpr auto short_bar = static_cast<int>(led_count * 0.50);

	sequential_animation<animation_loop, to_grb888,
		repeat<plain_color_animation<0x00, 0x00, 0xff>, long_bar>,
		repeat<plain_color_animation<0xff, 0x20, 0x00>, short_bar>,
		repeat<plain_color_animation<0x00, 0x00, 0xff>, long_bar>,
		repeat<plain_color_animation<0x00, 0xff, 0x20>, short_bar>>();
}

int main() {
	initialize();

	for (;;) {
		forwarding_light_strips();
	}

	return 0;
}
