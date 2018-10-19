#pragma once

#include <animation/animation.hpp>
#include <types.hpp>

class rainbow_animation {
	enum class rainbow_transition {
		from_red_to_green,
		from_green_to_blue,
		from_blue_to_red,
	};

public:
	constexpr auto value() const noexcept {
		return m_value;
	}

	constexpr void step() {
		one_iterration();
	}

	constexpr bool is_finished() const noexcept {
		return m_finished;
	}

private:
	constexpr void one_iterration() {
		if (m_state == rainbow_transition::from_red_to_green) {
			m_value.r--;
			m_value.g++;

			if (m_value.g == 0xff) {
				m_state = rainbow_transition::from_green_to_blue;
				m_value.r = 0;
				m_value.b = 0;
				return;
			}
		}

		if (m_state == rainbow_transition::from_green_to_blue) {
			m_value.b++;
			m_value.g--;

			if (m_value.b == 0xff) {
				m_state = rainbow_transition::from_blue_to_red;
				m_value.g = 0;
				m_value.r = 0;
				return;
			}
		}

		if (m_state == rainbow_transition::from_blue_to_red) {
			m_value.b--;
			m_value.r++;

			if (m_value.r == 0xff) {
				m_state = rainbow_transition::from_red_to_green;
				m_value.b = 0;
				m_value.g = 0;
				m_finished = true;
				return;
			}
		}
	}

private:
	rainbow_transition m_state{rainbow_transition::from_red_to_green};
	rgb m_value{0xff, 0x00, 0x00};
	bool m_finished{false};
};
