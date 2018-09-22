#pragma once

constexpr auto cycles_per_sec = F_CPU;

template <auto time>
struct time_unit {
	static constexpr auto value = time;

	static constexpr auto to_cycles(decltype(time) value) {
		return value / (time / cycles_per_sec);
	}
};

using ms = time_unit<1000>;
using us = time_unit<1000000>;
using ns = time_unit<1000000000>;
