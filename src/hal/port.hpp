#pragma once

#include "types.hpp"

enum class port_access_strategy {
	mem,
	io,
};

template <auto addr, auto bit>
class port_mem {
public:
	static void hi() {
		*reinterpret_cast<volatile u8 *>(addr) |= static_cast<u8>(1 << bit);
	}

	static void lo() {
		*reinterpret_cast<volatile u8 *>(addr) &= ~static_cast<u8>(1 << bit);
	}
};

template <auto addr, auto bit>
class port_io {
	static constexpr auto addr_shift = 0x20;

public:
	static void hi() {
#ifdef __AVR__
		asm volatile(
			"sbi %[addr], %[bit] \n\t" ::
				[addr] "I"(addr),
			[bit] "I"(bit));
#else
		port_mem<addr, bit>::hi();
#endif
	}

	static void lo() {
#ifdef __AVR__
		asm volatile(
			"cbi %[addr], %[bit] \n\t" ::
				[addr] "I"(addr),
			[bit] "I"(bit));
#else
		port_mem<addr, bit>::lo();
#endif
	}
};

template <auto addr, auto bit, port_access_strategy strategy = port_access_strategy::io>
class port {
public:
	static void hi() {
		if constexpr (port_access_strategy::io == strategy) {
			port_io<addr, bit>::hi();
		}

		if constexpr (port_access_strategy::mem == strategy) {
			port_mem<addr, bit>::hi();
		}
	}

	static void lo() {
		if constexpr (port_access_strategy::io == strategy) {
			port_io<addr, bit>::lo();
		}

		if constexpr (port_access_strategy::mem == strategy) {
			port_mem<addr, bit>::lo();
		}
	}

	static bool state() {
		return (*reinterpret_cast<volatile u8 *>(addr) & static_cast<u8>(1 << bit)) == static_cast<u8>(1 << bit);
	}
};

class uart {

public:
	template <typename value_type = u8>
	static value_type recv() noexcept {
		value_type value;

		for (auto i = 0u; i < sizeof(value); ++i)
			reinterpret_cast<u8 *>(&value)[i] = recv_8u();

		return value;
	}

	template <typename value_type>
	static void send(value_type &&value) {
		for (auto i = 0u; i < sizeof(value); ++i)
			send_8u(reinterpret_cast<const u8 *>(&value)[i]);
	}

	static void send(const char *line) {
		for (auto i = 0u; line[i] != '\0'; ++i)
			send_8u(static_cast<u8>(line[i]));
	}

	static u16 recv_16u() noexcept {
		u16 ret{};
		ret |= recv_8u() << 8;
		ret |= recv_8u();
		return ret;
	}

	static u8 recv_8u() noexcept {
		loop_until_bit_is_set(UCSR0A, RXC0);
		return UDR0;
	}

	static void send_8u(const u8 value) noexcept {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = value;
	}
};
