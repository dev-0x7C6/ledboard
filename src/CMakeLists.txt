include_directories(
	${CMAKE_SOURCE_DIR}/src/
	${CMAKE_SOURCE_DIR}/src/etl/include/etl
	${CMAKE_SOURCE_DIR}/src/etl/include
    hal
)

add_avr_executable(
   wsled
   main.cpp
   hal/port.hpp
   hal/pwm.hpp
   types.hpp
   hal/ws2812b.hpp
   hal/traits.hpp
   color.hpp
   palette.hpp
   animation/animation.hpp
   animation/rainbow_animation.hpp
)

target_link_libraries(
   wsled-${AVR_MCU}.elf
)
