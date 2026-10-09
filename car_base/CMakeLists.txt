cmake_minimum_required(VERSION 3.13)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

# ===== Force Ninja response file (avoid Chinese path encoding issues on Windows) =====
set(CMAKE_NINJA_FORCE_RESPONSE_FILE ON CACHE INTERNAL "")

# ===== Toolchain =====
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)
set(CMAKE_EXECUTABLE_SUFFIX .elf)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(my_car C ASM)

# ===== MSPM0 SDK path =====
set(SDK_DIR D:/32/ti/mspm0_sdk_2_11_00_07/source)
set(DL_LIB  ${SDK_DIR}/ti/driverlib/lib/gcc/m0p/mspm0g1x0x_g3x0x/driverlib.a)

# ===== Chip defines =====
set(MCU_FLAGS_STR "-mcpu=cortex-m0plus -mthumb")
separate_arguments(MCU_FLAGS UNIX_COMMAND ${MCU_FLAGS_STR})

set(DEFINES_STR "__MSPM0G3507__ __USE_SYSCONFIG__")
separate_arguments(DEFINES UNIX_COMMAND ${DEFINES_STR})

# ===== Compile options =====
add_compile_options(${MCU_FLAGS})
add_compile_options(-std=gnu11 -Wall -Wextra -g -O0)
add_compile_options(-ffunction-sections -fdata-sections)
foreach(d ${DEFINES})
    add_compile_options(-D${d})
endforeach()

# ===== Link options =====
add_link_options(${MCU_FLAGS})
add_link_options(-T ${CMAKE_SOURCE_DIR}/config/device_linker.lds)
add_link_options(-Wl,-Map=${PROJECT_NAME}.map,--cref)
add_link_options(-Wl,--gc-sections)
add_link_options(--specs=nosys.specs -nostartfiles)

# ===== Source files (auto-discover from src/ directories) =====
file(GLOB_RECURSE PROJECT_SOURCES
    "src/*.c"
    "src/*.s"
    "src/*.S"
)

# ===== Executable =====
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    config/ti_msp_dl_config.c
    ${SDK_DIR}/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c
)

# ===== Include paths =====
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/app
    ${CMAKE_SOURCE_DIR}/inc/BSP
    ${CMAKE_SOURCE_DIR}/inc/driver
    ${CMAKE_SOURCE_DIR}/inc/modules
    ${CMAKE_SOURCE_DIR}/inc/utils
    ${SDK_DIR}
    ${SDK_DIR}/third_party/CMSIS/Core/Include
)

# ===== Link driverlib =====
target_link_libraries(${PROJECT_NAME} PRIVATE ${DL_LIB} m)

# ===== Post-build: .hex (flash prog) + .bin (bootloader) + size =====
# Use WORKING_DIRECTORY + relative names to avoid Chinese path encoding issues in cmd.exe
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE_NAME:${PROJECT_NAME}>
            ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE_NAME:${PROJECT_NAME}>
            ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE_NAME:${PROJECT_NAME}>
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating hex, bin and size info"
)