#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <animation/rainbow_animation.hpp>
#include <hal/regs.hpp>
#include <hal/ws2812b.hpp>
#include <pwm.hpp>

#include <etl/array.h>

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
		sequential_animation<animation_loop, palette_converter_wrapper<rainbow_animation<1>, palette_category::grb888>>();
	}
	return 0;
}
