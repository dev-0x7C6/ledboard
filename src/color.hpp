#pragma once

#include <array.h>
#include <types.hpp>

using color = u8;
template <size_t size>
using color_array = etl::array<color, size>;

struct rgb {
	color r;
	color g;
	color b;
};
