#pragma once

#include "port.hpp"

template <auto cycles>
constexpr void nop_section() {
	asm volatile(
		".rept %[cycles] \n\t"
		"nop \n\t"
		".endr \n\t" ::
			[cycles] "I"(cycles));
}

template <auto addr, auto bit, port_access_strategy strategy, template <auto, auto, port_access_strategy> class port_type_template = port>
class pwm {
	using port_type = port<addr, bit, strategy>;

public:
	pwm(port_type &port)
			: m_port(port){};

	template <auto hi_cycles, auto lo_cycles>
	void generate() {
		m_port.template hi();
		nop_section<hi_cycles>();
		m_port.template lo();
		nop_section<lo_cycles>();
	}

	port_type &port_() { return m_port; }

private:
	port_type &m_port;
};
