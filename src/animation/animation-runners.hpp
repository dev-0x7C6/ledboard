#pragma once

#include <animation/animation.hpp>

template <auto callback, animation_interface animation_type>
void run_annimation() {
	animation_type animation;

	while (!animation.is_finished()) {
		animation.step();
		callback(animation.value());
	}
}

template <auto callback, animation_interface animation_type, animation_interface... args>
constexpr void sequential_animation() noexcept {
	run_annimation<callback, animation_type>();

	if constexpr (sizeof...(args) > 0) {
		sequential_animation<callback, args...>();
	}
}

template <auto callback, template <animation_interface> class wrapper, animation_interface animation_type, animation_interface... args>
constexpr void sequential_animation() noexcept {
	run_annimation<callback, wrapper<animation_type>>();

	if constexpr (sizeof...(args) > 0) {
		sequential_animation<callback, wrapper, args...>();
	}
}

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
