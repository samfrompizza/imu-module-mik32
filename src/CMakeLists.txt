# Setting up app build
set(APP_NAME ${CMAKE_PROJECT_NAME})

add_executable(${APP_NAME}
    main.c
    crt.S

    drivers/bno055_port.c
    drivers/button.c
    drivers/display_service.c
    drivers/imu_service.c
    drivers/timer_service.c
    drivers/trap_handler.c

    app/mode_manager.c
    app/mode_welcome.c
    app/mode_debug.c
    app/mode_level.c
    app/mode_compass.c
    app/mode_ball.c
)

target_include_directories(${APP_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${APP_NAME}
    PRIVATE
        mik32_hal
        bno055
        m
)

target_link_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/hardware/mik32v2-shared/ldscripts
)

# Convert output to hex and binary
add_custom_command(TARGET ${APP_NAME}
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy ARGS -O binary ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX_C} ${APP_NAME}.bin
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy ARGS -O ihex ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX_C} ${APP_NAME}.hex
    COMMAND ${TOOLCHAIN_PREFIX}-objdump -w -x -s -S ${APP_NAME}.elf > ${APP_NAME}.dump
)