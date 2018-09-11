#pragma once

#include "cpu/speed.h"
#include "traits.hpp"
#include "pwm.hpp"

enum class ws_type {
	ws2812b,
	ws2812
};

enum class palette {
	rgb,
	rbg,
	bgr,
	brg,
	grb,
	gbr,
};

class ws_timing_container {
	using pair_type = pair<int>;

public:
	constexpr ws_timing_container(pair_type hi, pair_type lo, int reset)
			: m_hi(hi)
			, m_lo(lo)
			, m_reset(reset) {}

	constexpr auto hi_phase() const noexcept { return m_hi; }
	constexpr auto lo_phase() const noexcept { return m_lo; }
	constexpr auto reset_phase() const noexcept { return m_reset; }

private:
	const pair_type m_hi;
	const pair_type m_lo;
	const int m_reset;
};

constexpr auto get_ws_timings(ws_type value) noexcept -> ws_timing_container {
	switch (value) {
		case ws_type::ws2812b: return {{900, 600}, {400, 900}, 10000};
		case ws_type::ws2812: return {{700, 350}, {600, 600}, 30000};
	}

	return get_ws_timings(ws_type::ws2812b);
}

constexpr auto get_ws_palette(ws_type value) noexcept -> palette {
	switch (value) {
		case ws_type::ws2812b: return palette::grb;
		case ws_type::ws2812: return palette::rgb;
	}
}

template <auto addr, auto bit, port_access_strategy strategy, ws_type driver = ws_type::ws2812, template <auto, auto, port_access_strategy> class pwm_type_template = pwm>
class ws2812 {
	using pwm_type = pwm_type_template<addr, bit, strategy>;

public:
	ws2812(pwm_type &pwm)
			: m_pwm(pwm) {}

	void write(u8 *begin, int size) {
		const u8 *end = begin + size;

		while (begin != end) {
			u8 color = *begin;
			for (u8 i = 0; i < 8; ++i) {
				write(color & (1 << 7));
				color <<= 1;
			}
			begin++;
		}

		m_pwm.template port_().lo();
		constexpr auto reset_phase = ns::to_cycles(get_ws_timings(driver).reset_phase());
		__builtin_avr_delay_cycles(reset_phase);
	}

	constexpr auto palette() const noexcept { return get_ws_palette(driver); }

private:
	void write(bool value) {
		constexpr auto hi_phase = get_ws_timings(driver).hi_phase();
		constexpr auto lo_phase = get_ws_timings(driver).lo_phase();

		if (value) {
			m_pwm.template generate<ns::to_cycles(hi_phase.p1()) - 2, ns::to_cycles(hi_phase.p2()) - 2>();
		} else {
			m_pwm.template generate<ns::to_cycles(lo_phase.p1()) - 2, ns::to_cycles(lo_phase.p2()) - 2>();
		}
	}

private:
	pwm_type &m_pwm;
};

template <auto addr, auto bit, port_access_strategy strategy, template <auto, auto, port_access_strategy> class pwm_type_template = pwm>
class ws2812b : public ws2812<addr, bit, strategy, ws_type::ws2812b, pwm_type_template> {
	using pwm_type = pwm_type_template<addr, bit, strategy>;

public:
	ws2812b(pwm_type &pwm)
			: ws2812<addr, bit, strategy, ws_type::ws2812b, pwm_type_template>(pwm) {}
};
