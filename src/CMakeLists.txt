include_directories(
	${CMAKE_SOURCE_DIR}/src/
	${CMAKE_SOURCE_DIR}/src/externals/etl/include/etl
	${CMAKE_SOURCE_DIR}/src/externals/etl/include
	${CMAKE_SOURCE_DIR}/src/externals/common
    hal
)

set(hal-sources
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
   animation/generators/plain-color-animation.hpp
)

add_avr_executable(
   ledframe
   ledframe.cpp
   ${hal-sources}
)

add_avr_executable(
   ledboard
   ledboard.cpp
   ${hal-sources}
)

target_link_libraries(
   ledframe-${AVR_MCU}.elf
)

target_link_libraries(
   ledboard-${AVR_MCU}.elf
)

