add_executable(
  slotgame
  slotgame.cpp
  slotlib.cpp
)

# Pull in pico libraries that we need
target_link_libraries(slotgame pico_stdlib hardware_spi hardware_pwm hardware_dma pico_display)

# enable usb output, disable uart output
#pico_enable_stdio_usb(slotgamek 1)
#pico_enable_stdio_uart(slotgamek 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(slotgame)
