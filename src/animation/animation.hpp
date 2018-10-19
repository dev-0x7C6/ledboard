#pragma once

#include <color.hpp>
#include <palette.hpp>

// clang-format off

template<typename type>
concept bool animation_interface = requires(type object) {
    { object.value() } -> rgb
    { object.step() } -> void
    { object.is_finished() } -> bool
};

// clang-format on

template <auto callback, animation_interface animation_type>
void run_annimation() {
	animation_type animation;

	while (!animation.is_finished()) {
		animation.step();
		callback(animation.value());
	}
}

template <auto callback, animation_interface animation_type, animation_interface... args>
void sequential_animation() {
	run_annimation<callback, animation_type>();

	if constexpr (sizeof...(args) > 0) {
		sequential_animation<callback, args...>();
	}
}

template <animation_interface animation_type, palette_category category>
class palette_converter_wrapper : public animation_type { // eventually operator. (aka dot) overload
public:
	constexpr auto value() const noexcept {
		return convert_palette<category>(animation_type::value());
	}
};

template <animation_interface animation>
using to_grb888 = palette_converter_wrapper<animation, palette_category::grb888>;

template <animation_interface animation>
using to_gbr888 = palette_converter_wrapper<animation, palette_category::gbr888>;

template <animation_interface animation>
using to_rgb888 = palette_converter_wrapper<animation, palette_category::rgb888>;

template <animation_interface animation>
using to_rbg888 = palette_converter_wrapper<animation, palette_category::rbg888>;

template <animation_interface animation>
using to_bgr888 = palette_converter_wrapper<animation, palette_category::bgr888>;

template <animation_interface animation>
using to_brg888 = palette_converter_wrapper<animation, palette_category::brg888>;

template <animation_interface animation_type, auto times>
class repeat { // eventually operator. (aka dot) overload
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

template <auto elements>
class transformation {
public:
	constexpr static void rotate(rgb *colors, rgb value) {
		for (int i = elements - 1; i > 0; i--)
			colors[i] = colors[i - 1];
		colors[0] = value;
	}

	constexpr static void fill(rgb *colors, rgb value) {
		for (int i = 0; i < elements; ++i)
			colors[i] = value;
	}
};
