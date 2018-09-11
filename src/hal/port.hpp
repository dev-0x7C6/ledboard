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

template <auto addr>
class uart {
public:
	u8 recv() const {
		loop_until_bit_is_set(UCSR0A, RXC0);
		return UDR0;
	}

	void send(u8 value) {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = value;
	}
};
