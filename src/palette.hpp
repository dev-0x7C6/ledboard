#pragma once

#include <color.hpp>

enum class palette_category {
	rgb,
	rbg,
	bgr,
	brg,
	grb,
	gbr,
};

template <palette_category category>
constexpr void insert_color(color *data, rgb &&value) {
	if constexpr (category == palette_category::gbr) {
		data[0] = value.g;
		data[1] = value.b;
		data[2] = value.r;
	}

	if constexpr (category == palette_category::grb) {
		data[0] = value.g;
		data[1] = value.r;
		data[2] = value.b;
	}

	if constexpr (category == palette_category::rgb) {
		data[0] = value.r;
		data[1] = value.g;
		data[2] = value.b;
	}

	if constexpr (category == palette_category::rbg) {
		data[0] = value.r;
		data[1] = value.b;
		data[2] = value.g;
	}

	if constexpr (category == palette_category::brg) {
		data[0] = value.b;
		data[1] = value.g;
		data[2] = value.r;
	}

	if constexpr (category == palette_category::bgr) {
		data[0] = value.b;
		data[1] = value.g;
		data[2] = value.r;
	}
}
