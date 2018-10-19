#pragma once

#include <animation/animation.hpp>

template <animation_interface animation_type, palette_category category>
class convert : public animation_type { // eventually operator. (aka dot) overload
public:
	constexpr auto value() const noexcept {
		return convert_palette<category>(animation_type::value());
	}
};

template <animation_interface animation>
using to_grb888 = convert<animation, palette_category::grb888>;

template <animation_interface animation>
using to_gbr888 = convert<animation, palette_category::gbr888>;

template <animation_interface animation>
using to_rgb888 = convert<animation, palette_category::rgb888>;

template <animation_interface animation>
using to_rbg888 = convert<animation, palette_category::rbg888>;

template <animation_interface animation>
using to_bgr888 = convert<animation, palette_category::bgr888>;

template <animation_interface animation>
using to_brg888 = convert<animation, palette_category::brg888>;

template <animation_interface animation_type, auto times>
class repeat {
public:
	constexpr auto value() const noexcept {
		return m_animation.value();
	}

	void step() {
		m_animation.step();
		if (m_animation.is_finished()) {
			m_finished = (--m_steps) == 0;
			if (!m_finished)
				m_animation = decltype(m_animation){};
		}
	}

	constexpr bool is_finished() const noexcept {
		return m_finished;
	}

private:
	int m_steps = times;
	animation_type m_animation;
	bool m_finished{false};
};

template <animation_interface animation_type, auto steps>
class speed : public animation_type { // eventually operator. (aka dot) overload
public:
	void step() {
		for (int i = 0; i < steps; ++i)
			if (!animation_type::is_finished())
				animation_type::step();
	}
};
