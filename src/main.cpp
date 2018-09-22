#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "animation/rainbow_animation.hpp"
#include "hal/regs.hpp"
#include "hal/ws2812b.hpp"
#include <pwm.hpp>

constexpr auto led_count = 38;
constexpr auto led_type = ws_type::ws2812b;

template <auto palette_type = palette::gbr>
constexpr void insert_color(u8 *data, rgb &&value) {
	if constexpr (palette_type == palette::gbr) {
		data[0] = value.g;
		data[1] = value.b;
		data[2] = value.r;
	}

	if constexpr (palette_type == palette::grb) {
		data[0] = value.g;
		data[1] = value.r;
		data[2] = value.b;
	}

	if constexpr (palette_type == palette::rgb) {
		data[0] = value.r;
		data[1] = value.g;
		data[2] = value.b;
	}

	if constexpr (palette_type == palette::rbg) {
		data[0] = value.r;
		data[1] = value.b;
		data[2] = value.g;
	}

	if constexpr (palette_type == palette::brg) {
		data[0] = value.b;
		data[1] = value.g;
		data[2] = value.r;
	}

	if constexpr (palette_type == palette::bgr) {
		data[0] = value.b;
		data[1] = value.g;
		data[2] = value.r;
	}
}

// clang-format off

template<typename type>
concept bool animation_interface = requires(type a) {
    { a.value() } -> rgb
    { a.r() } -> u8
    { a.g() } -> u8
    { a.b() } -> u8
    { a.step() } -> void
};

// clang-format on

template <animation_interface animation_type, palette palette_variant, auto elements>
class animator {
public:
	animator(rgb *c)
			: m_colors(c) {}

	void rotate(int count = 1) {
		for (int i = elements - 1; i > 0; i--)
			m_colors[i] = m_colors[i - 1];
		m_colors[0] = animation.value();
		steps(count);
	}

private:
	constexpr void steps(int count) noexcept {
		for (int i = 0; i < count; ++i)
			animation.step();
	}

private:
	animation_type animation;
	rgb *m_colors;
};

int main() {
	u8 data[led_count * 3] = {};

	port<regs::ddr_d, 5>::hi();
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);

	animator<rainbow_animation, ws.palette(), led_count> animator(reinterpret_cast<rgb *>(data));

	for (;;) {
		animator.rotate(5);
		ws.write(data, sizeof(data));
		_delay_ms(20);
	}

	return 0;
}
