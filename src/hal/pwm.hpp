#pragma once

#include "port.hpp"

enum class architecture {
	avr,
	arm,
	undefined,
};

template<auto arch>
concept bool compatible_with_arch() {
#ifdef __AVR__
	return architecture::avr == arch;
#elif __arm__
	return architecture::arm == arch;
#else
	return architecture::undefined == arch;
#endif
}

template <auto cycles>
requires compatible_with_arch<architecture::avr>()
void nop_section() {
	asm volatile (
		".rept %[cycles] \n\t"
		"nop \n\t"
		".endr \n\t"
		::                
		[cycles]	"I" (cycles)
	);
}

template <auto cycles>
requires compatible_with_arch<architecture::arm>()
void nop_section() {
	asm volatile (
		".rept %[cycles] \n\t"
		"nop \n\t"
		".endr \n\t"
		::                
		[cycles]	"I" (cycles)
	);
}

template <auto cycles>
requires compatible_with_arch<architecture::undefined>()
void __avr_generate_nop_section() {
}

template <auto addr, auto bit, port_access_strategy strategy, template <auto, auto, port_access_strategy> class port_type_template = port>
class pwm
{
	using port_type = port<addr, bit, strategy>;
	
public:
	pwm(port_type &port) : m_port(port) {};
	
	template <auto hi_cycles, auto lo_cycles>
	void generate() {
		m_port.template hi();
		nop_section<hi_cycles>();
		m_port.template lo();
		nop_section<lo_cycles>();
	}
	
	port_type& port_() { return m_port; }
	
private:
	port_type &m_port;
};
