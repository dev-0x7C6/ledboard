include_directories(
	${CMAKE_SOURCE_DIR}/src/
	${CMAKE_SOURCE_DIR}/src/externals/etl/include/etl
	${CMAKE_SOURCE_DIR}/src/externals/etl/include
	${CMAKE_SOURCE_DIR}/src/externals/common
    hal
)

add_avr_executable(
   wsled
   main.cpp
   hal/port.hpp
   hal/pwm.hpp
   externals/common/types.hpp
   hal/ws2812b.hpp
   hal/traits.hpp
   color.hpp
   palette.hpp
   animation/animation.hpp
   animation/animation-runners.hpp
   animation/animation-modifiers.hpp
   animation/generators/rainbow-animation.hpp
)

target_link_libraries(
   wsled-${AVR_MCU}.elf
)
