#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

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

void push_leds() {
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);
	ws.write(data);
	_delay_ms(30);
}

void animation_loop(const rgb value) noexcept {
	transformation<led_count>::rotate(reinterpret_cast<rgb *>(data.data()), value);
	push_leds();
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

void uart_send(const u8 byte) {
	while ((UCSR0A & (1 << UDRE0)) == 0)
		;
	UDR0 = byte;
}

u8 uart_recv() {
	while ((UCSR0A & (1 << RXC0)) == 0)
		;
	return UDR0;
}

template <typename T>
concept bool serial_device = requires(T a) {
	{ a.send({}) }
	->void;
	{ a.recv_8u() }
	->u8;
};

bool expect_sequence(serial_device &serial, const char *str) {
	for (auto i = 0; str[i] != '\0'; ++i)
		if (str[i] != serial.recv_8u())
			return false;

	return true;
}

void send(serial_device &serial, const char *str) {
	for (auto i = 0; str[i] != '\0'; ++i)
		serial.send(str[i]);
}

int main() {
	initialize();

	UBRR0H = 0x00;
	UBRR0L = 0x02;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	uart serial;
	u16 error_count{};

	push_leds();

	for (;;) {
		if (!expect_sequence(serial, "LED"))
			continue;

		const auto expected_led_count = serial.recv_16u();
		const auto expected_sum = serial.recv_16u();

		u16 local_sum{};

		for (auto i = 0u; i < expected_led_count; ++i) {
			data[i] = serial.recv_8u();
			local_sum += data[i];
		}

		if (expected_sum != local_sum) {
			error_count++;
		}

		push_leds();

		send(serial, "RDY");
	}

	return 0;
}
