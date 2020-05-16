#pragma once

#include <color.hpp>
#include <traits.hpp>

enum class palette_category {
	rgb888,
	rbg888,
	grb888,
	gbr888,
	bgr888,
	brg888,
};

template <typename T>
concept palette_compatible = requires(T a) {
	{ rgb({a}) }
	->same_as<rgb>; // convertable to rgb, for example from initializer_list
};

template <palette_category category, palette_compatible type> // generate efficent signature for every value category, still universal reference :)
constexpr auto convert_palette(type &&value) noexcept -> rgb {
	// reflection and 'for...' code injection, probably >=C++23
	if constexpr (category == palette_category::gbr888)
		return {value.g, value.b, value.r};
	if constexpr (category == palette_category::grb888)
		return {value.g, value.r, value.b};
	if constexpr (category == palette_category::rgb888)
		return {value.r, value.g, value.b};
	if constexpr (category == palette_category::rbg888)
		return {value.r, value.b, value.g};
	if constexpr (category == palette_category::brg888)
		return {value.b, value.r, value.g};
	if constexpr (category == palette_category::bgr888)
		return {value.b, value.g, value.r};
}

template <palette_category category>
constexpr auto convert_palette_test(rgb &&from, rgb &&to) {
	return convert_palette<category>(from) == to;
}

static_assert(convert_palette_test<palette_category::gbr888>({0xab, 0xcd, 0xef}, {0xcd, 0xef, 0xab}));
static_assert(convert_palette_test<palette_category::grb888>({0xab, 0xcd, 0xef}, {0xcd, 0xab, 0xef}));
static_assert(convert_palette_test<palette_category::rgb888>({0xab, 0xcd, 0xef}, {0xab, 0xcd, 0xef}));
static_assert(convert_palette_test<palette_category::rbg888>({0xab, 0xcd, 0xef}, {0xab, 0xef, 0xcd}));
static_assert(convert_palette_test<palette_category::brg888>({0xab, 0xcd, 0xef}, {0xef, 0xab, 0xcd}));
static_assert(convert_palette_test<palette_category::bgr888>({0xab, 0xcd, 0xef}, {0xef, 0xcd, 0xab}));
