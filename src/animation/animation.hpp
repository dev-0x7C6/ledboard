#pragma once

#include <color.hpp>
#include <palette.hpp>
#include <traits.hpp>

enum class Infill {
	RotateLeft,
	RotateRight,
	Fill
};

template <typename type>
concept animation_interface = requires(type object) {
	{ object.value() }
	->same_as<rgb>;
	{ object.step() }
	->same_as<void>;
	{ object.is_finished() }
	->same_as<bool>;
};
