add_library(PS2_DEVICE_lib INTERFACE)
target_include_directories(PS2_DEVICE_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(PS2_DEVICE_lib INTERFACE hardware_pio)
target_sources(PS2_DEVICE_lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/PS2_keyboard_device.c
    ${CMAKE_CURRENT_LIST_DIR}/PS2_mouse_device.c
    ${CMAKE_CURRENT_LIST_DIR}/PS2_pio_com.c
    ${CMAKE_CURRENT_LIST_DIR}/PS2_com.pio)
pico_generate_pio_header(PS2_DEVICE_lib ${CMAKE_CURRENT_LIST_DIR}/PS2_com.pio)
