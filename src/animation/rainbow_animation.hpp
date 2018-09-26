#pragma once

#include "types.hpp"
#include <animation/animation.hpp>

class rainbow_animation {
	enum class rainbow_transition {
		from_red_to_green,
		from_green_to_blue,
		from_blue_to_red,
	};

public:
	constexpr auto r() const noexcept { return m_r; }
	constexpr auto g() const noexcept { return m_g; }
	constexpr auto b() const noexcept { return m_b; }
	constexpr auto value() const noexcept {
		return rgb{m_r, m_g, m_b};
	}
	
	constexpr void steps(int count) noexcept {
		for (int i = 0; i < count; ++i)
			step();
	}

	constexpr void step() {
		if (m_state == rainbow_transition::from_red_to_green) {
			m_r--;
			m_g++;

			if (m_g == 0xff) {
				m_state = rainbow_transition::from_green_to_blue;
				m_r = 0;
				m_b = 0;
				return;
			}
		}

		if (m_state == rainbow_transition::from_green_to_blue) {
			m_b++;
			m_g--;

			if (m_b == 0xff) {
				m_state = rainbow_transition::from_blue_to_red;
				m_g = 0;
				m_r = 0;
				return;
			}
		}

		if (m_state == rainbow_transition::from_blue_to_red) {
			m_b--;
			m_r++;

			if (m_r == 0xff) {
				m_state = rainbow_transition::from_red_to_green;
				m_b = 0;
				m_g = 0;
				return;
			}
		}
	}

private:
	rainbow_transition m_state{rainbow_transition::from_red_to_green};
	u8 m_r{0xff};
	u8 m_g{0x00};
	u8 m_b{0x00};
};
