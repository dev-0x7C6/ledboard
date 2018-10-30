#pragma once

#include <animation/animation.hpp>
#include <types.hpp>

template <auto r, auto g, auto b>
class plain_color_animation {
public:
	constexpr auto value() const noexcept {
		return rgb{r, g, b};
	}

	constexpr void step() {
	}

	constexpr bool is_finished() const noexcept {
		return true;
	}
};
