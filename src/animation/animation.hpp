#pragma once

#include <color.hpp>
#include <palette.hpp>

// clang-format off

template<typename type>
concept bool animation_interface = requires(type object) {
    { object.value() } -> rgb
    { object.r() } -> u8
    { object.g() } -> u8
    { object.b() } -> u8
    { object.step() } -> void
};

// clang-format on

template <animation_interface animation_type, palette_category category>
class palette_converter_wrapper : public animation_type { // eventualy operator. (aka dot) overload
public:
	constexpr auto value() const noexcept {
		return convert_palette<category>(animation_type::value());
	}
};

template <animation_interface animation_type, auto elements>
class animator {
public:
	constexpr animator(rgb *c)
			: m_colors(c) {}

	constexpr void rotate(int count = 1) {
		for (int i = elements - 1; i > 0; i--)
			m_colors[i] = m_colors[i - 1];
		m_colors[0] = m_animation.value();
		steps(count);
	}

private:
	constexpr void steps(int count) noexcept {
		for (int i = 0; i < count; ++i)
			m_animation.step();
	}

private:
	animation_type m_animation;
	rgb *m_colors;
};
