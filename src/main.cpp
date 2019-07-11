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

#include <string.h>

constexpr auto led_count = 100;

static color_array<led_count * 3> data{};
static u16 error_count{};

void initialize() {
	port<regs::ddr_d, 5>::hi();
}

template <auto delay = 0>
void push_leds() {
	port<regs::portd, 5> ws_port;
	pwm ws_pwm(ws_port);
	ws2812b ws(ws_pwm);
	ws.write(data);
	if constexpr (delay > 0)
		_delay_ms(delay);
}

template <auto delay = 30>
void animation_loop(const rgb value) noexcept {
	transformation<led_count>::rotate(reinterpret_cast<rgb *>(data.data()), value);
	push_leds<delay>();
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
	{ a.recv() }
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

namespace proto {

enum class command : u8 {
	undef = 0x00,
	init = 'i',
	done = 'd',
	push = 'p',
	info = 'n',
	sync = 's',
	clear = 'c',
};

enum class response {
	none,
	ready,
	synced,
};

struct lf_push_params {
	u16 led_count;
	u16 sum;
};

struct lf_version {
	u8 version{1};
};

struct lf_info {
	char name[32];
	char conf[64];
};

}

static_assert(sizeof(proto::lf_push_params) == 4);

constexpr auto to_command(char *buffer) noexcept {
	constexpr auto command_prefix = "LF_";

	if (strncmp(buffer, command_prefix, 3))
		return proto::command::undef;

	return static_cast<proto::command>(buffer[3]);
}

template <typename buffer_type>
bool read_line(serial_device &serial, buffer_type &&buffer) {
	for (auto i = 0u; i < sizeof(buffer); ++i) {
		buffer[i] = serial.recv_8u();

		if (buffer[i] == '\0')
			return true;
	}

	return false;
}

proto::response on_command_push(uart &serial) {
	const auto params = serial.recv<proto::lf_push_params>();
	u16 local_sum{};

	for (auto i = 0u; i < params.led_count; ++i) {
		data[i] = serial.recv_8u();
		local_sum += data[i];
	}

	if (params.sum == local_sum)
		push_leds<0>();
	else
		error_count++;

	return proto::response::ready;
}

proto::response on_command_init(uart &) {
	return proto::response::ready;
}

proto::response on_command_done(uart &) {
	return proto::response::ready;
}

proto::response on_command_info(uart &) {
	return proto::response::ready;
}

proto::response on_command_sync(uart &) {
	return proto::response::synced;
}

proto::response on_command_clear(uart &) {
	return proto::response::ready;
}

void process_response(uart &serial, const proto::response rep) {
	switch (rep) {
		case proto::response::none:
			return;
		case proto::response::ready:
			serial.send('R');
			serial.send('D');
			serial.send('Y');
			serial.send('\0');
			return;
		case proto::response::synced:
			serial.send('S');
			serial.send('Y');
			serial.send('N');
			serial.send('C');
			serial.send('\0');
			return;
	}
}

auto process_command(uart &serial, const proto::command cmd) {
	switch (cmd) {
		case proto::command::push: return on_command_push(serial);
		case proto::command::sync: return on_command_sync(serial);
		case proto::command::info: return on_command_info(serial);
		case proto::command::init: return on_command_init(serial);
		case proto::command::clear: return on_command_clear(serial);
		case proto::command::done: return on_command_done(serial);
		case proto::command::undef: return proto::response::none;
	}

	return proto::response::none;
}

template <typename buffer_type>
void process(uart &serial, buffer_type &&buffer) {
	const auto cmd = to_command(buffer);
	const auto rep = process_command(serial, cmd);
	process_response(serial, rep);
}

int main() {
	initialize();

	sequential_animation<animation_loop<0>, to_grb888,
		repeat<plain_color_animation<0x00, 0x00, 0xff>, led_count / 1>,
		repeat<plain_color_animation<0xff, 0x20, 0x00>, led_count / 2>,
		repeat<plain_color_animation<0x00, 0x00, 0xff>, led_count / 1>,
		repeat<plain_color_animation<0x00, 0xff, 0x20>, led_count / 2>>();

	UBRR0H = 0x00;
	UBRR0L = 0x02;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	uart serial;

	for (;;) {
		constexpr auto cmd_buffer_size = 16;
		char buffer[cmd_buffer_size];
		if (!read_line(serial, buffer))
			continue;

		process(serial, buffer);
	}

	return 0;
}
