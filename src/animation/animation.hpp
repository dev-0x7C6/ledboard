#pragma once

#include <color.hpp>
#include <palette.hpp>

enum class Infill {
	RotateLeft,
	RotateRight,
	Fill
};

// clang-format off

template<typename type>
concept bool animation_interface = requires(type object) {
    { object.value() } -> rgb
    { object.step() } -> void
    { object.is_finished() } -> bool
};

// clang-format on
