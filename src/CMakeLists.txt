include_directories(
	${CMAKE_SOURCE_DIR}/src/
    hal
)

add_avr_executable(
   wsled
   main.cpp
   hal/port.hpp
   hal/pwm.hpp
   hal/types.hpp
   hal/ws2812b.hpp
   hal/traits.hpp
   animation/rainbow_animation.hpp
)

target_link_libraries(
   wsled-${AVR_MCU}.elf
)
