#pragma once

#include <color.hpp>
#include <palette.hpp>

// clang-format off

template<typename type>
concept bool animation_interface = requires(type a) {
    { a.value() } -> rgb
    { a.r() } -> u8
    { a.g() } -> u8
    { a.b() } -> u8
    { a.step() } -> void
};

// clang-format on

template <animation_interface animation_type, palette_category palette_category, auto elements>
class animator {
public:
	constexpr animator(rgb *c)
			: m_colors(c) {}

	constexpr void rotate(int count = 1) {
		for (int i = elements - 1; i > 0; i--)
			m_colors[i] = m_colors[i - 1];
		m_colors[0] = animation.value();
		steps(count);
	}

private:
	constexpr void steps(int count) noexcept {
		for (int i = 0; i < count; ++i)
			animation.step();
	}

private:
	animation_type animation;
	rgb *m_colors;
};
