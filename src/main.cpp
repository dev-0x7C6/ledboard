#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <animation/animation-runners.hpp>
#include <animation/animation-modifiers.hpp>
#include <animation/generators/rainbow-animation.hpp>
#include <animation/generators/plain-color-animation.hpp>

#include <hal/regs.hpp>
#include <hal/ws2812b.hpp>
#include <pwm.hpp>

#include <string.h>
#include <externals/protocol/protocol.hpp>

namespace {
constexpr auto ledframe_left_led_count = 18;
constexpr auto ledframe_top_led_count = 36;
constexpr auto ledframe_right_led_count = ledframe_left_led_count;
constexpr auto ledframe_bottom_led_count = ledframe_top_led_count;

constexpr auto led_count = ledframe_left_led_count + ledframe_top_led_count + ledframe_right_led_count + ledframe_bottom_led_count;
constexpr auto led_channel_count = 3;

color_array<led_count * led_channel_count> data{};
u16 error_count{};

constexpr char rdy_response[4] = {'R', 'D', 'Y', '\0'};
constexpr char sync_response[5] = {'S', 'Y', 'N', 'C', '\0'};

}

using namespace ledframe::proto;

constexpr auto make_configuration() noexcept {
	command_info_params params;
	params.version = 1;
	params.sequences = 4;
	strcpy(params.name, "Samsung TV");

	auto make_direction = [](const position pos, const u16 count, const order ord = order::clockwise) constexpr noexcept {
		strip_param param{};
		param.ord = static_cast<u8>(ord);
		param.pos = static_cast<u8>(pos);
		param.count = count;
		param.palette = static_cast<u8>(palette_category::grb888);
		return param;
	};

	params.directions[0] = make_direction(position::left, ledframe_left_led_count);
	params.directions[1] = make_direction(position::top, ledframe_top_led_count);
	params.directions[2] = make_direction(position::right, ledframe_right_led_count);
	params.directions[3] = make_direction(position::bottom, ledframe_bottom_led_count);
	return params;
}

void initialize() {
	port<regs::ddr_d, 5>::hi();
	UBRR0H = 0x00;
	UBRR0L = 0x02;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
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

template <typename T>
concept bool serial_device = requires(T a) {
	{ a.send_8u({}) }
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

constexpr auto to_command(char *buffer) noexcept {
	constexpr auto command_prefix = "LF_";

	if (strncmp(buffer, command_prefix, 3))
		return command::undef;

	return static_cast<command>(buffer[3]);
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

response on_command_push(uart &serial) {
	const auto params = serial.recv<command_push_params>();
	u16 local_sum{};

	if (params.led_count > sizeof(data))
		return response::none;

	const auto is_interlaced = params.flags.test(push_flags::id::interlaced);

	if (is_interlaced) {
		const auto is_even = params.flags.test(push_flags::id::interlaced_frame_even);
		auto index = (is_even ? 0u : led_channel_count);
		for (auto i = 0u; i < params.led_count; ++i) {
			for (auto channel = 0; channel < led_channel_count; ++channel)
				local_sum += (data[index + channel] = serial.recv_8u());
			index += led_channel_count * 2;
		}
	} else {
		for (auto i = 0u; i < params.led_count * led_channel_count; ++i)
			local_sum += (data[i] = serial.recv_8u());
	}

	if (params.sum == local_sum)
		push_leds();
	else
		error_count++;

	return response::ready;
}

response on_command_init(uart &) {
	return response::ready;
}

response on_command_done(uart &) {
	return response::ready;
}

response on_command_info(uart &serial) {
	auto cfg = make_configuration();
	serial.send(cfg);
	return response::ready;
}

response on_command_sync(uart &) {
	return response::synced;
}

response on_command_clear(uart &) {
	return response::ready;
}

void process_response(uart &serial, const response rep) {
	switch (rep) {
		case response::none:
			return;
		case response::ready:
			return serial.send(rdy_response);
		case response::synced:
			return serial.send(sync_response);
	}
}

auto process_command(uart &serial, const command cmd) {
	switch (cmd) {
		case command::push: return on_command_push(serial);
		case command::sync: return on_command_sync(serial);
		case command::info: return on_command_info(serial);
		case command::init: return on_command_init(serial);
		case command::clear: return on_command_clear(serial);
		case command::done: return on_command_done(serial);
		case command::undef: return response::none;
	}

	return response::none;
}

template <typename buffer_type>
void process(uart &serial, buffer_type &&buffer) {
	const auto cmd = to_command(buffer);
	const auto rep = process_command(serial, cmd);
	process_response(serial, rep);
}

int main() {
	initialize();
	uart serial;

	for (;;) {
		wdt_enable(WDTO_15MS);
		constexpr auto cmd_buffer_size = 16;
		char buffer[cmd_buffer_size];
		if (!read_line(serial, buffer))
			continue;

		process(serial, buffer);
		wdt_disable();
	}

	return 0;
}
