#pragma once

#include <array.h>
#include <types.hpp>
#include <etl/type_traits.h>
#include <hal/traits.hpp>

using color = u8;
template <size_t size>
using color_array = etl::array<color, size>;

struct rgb {
	color r;
	color g;
	color b;

	template <typename type>
	constexpr bool operator==(type &&rhs) const noexcept { return r == rhs.r && g == rhs.g && b == rhs.b; }

	template <typename type>
	constexpr bool operator!=(type &&rhs) const noexcept { return !operator==(forward<type>(rhs)); }
};
