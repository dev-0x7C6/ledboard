#pragma once

/*
 * AVR library delivers many expresions rather than constant variables, so there is a problem to get
 * address of common ports, registers in compile-time and assign them to constexpr constant or use
 * as template parameters
 * 
 * With help of my friend, I used this ugly macro that probably will generate a problem
 * in short period of time. I think it's better than hardcoded values anyway.
 */

constexpr uint16_t extract_addr_from_expresion(const char *r) {
	while (*r != 'x') r++;
	r++;
	uint16_t val = 0;
	while (*r != ')') {
		val = 16 * val + ((*r <= '9') ? (*r - '0') : ((*r & 0xdf) - 'A' + 10));
		r++;
	}
	return val;
}

#define VAL_EXTRACT_ADDR_FROM_EXPRESION(X) extract_addr_from_expresion(#X)
#define EXTRACT_ADDR(X) VAL_EXTRACT_ADDR_FROM_EXPRESION(X)

namespace regs {
using word_size = u16;
constexpr word_size ddr_d = EXTRACT_ADDR(DDRD);
constexpr word_size ddr_b = EXTRACT_ADDR(DDRB);
constexpr word_size portd = EXTRACT_ADDR(PORTD);
constexpr word_size portb = EXTRACT_ADDR(PORTB);
}
