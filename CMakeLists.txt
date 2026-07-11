cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

# arm-none-eabi-gcc toolchain
if(CMAKE_HOST_WIN32)
	set(SUFFIX ".exe")
else()
	set(SUFFIX "")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(NO_CACHE CROSS_GCC_PATH ${TOOLCHAIN_PREFIX}gcc${SUFFIX})
get_filename_component(TOOLCHAIN ${CROSS_GCC_PATH} PATH)

set(CMAKE_C_COMPILER ${TOOLCHAIN}/${TOOLCHAIN_PREFIX}gcc${SUFFIX})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/${TOOLCHAIN_PREFIX}g++${SUFFIX})
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(CMAKE_OBJCOPY ${TOOLCHAIN}/${TOOLCHAIN_PREFIX}objcopy${SUFFIX} CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${TOOLCHAIN}/${TOOLCHAIN_PREFIX}size${SUFFIX} CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Project

project(PlayBrew LANGUAGES C ASM)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(HEAP_SIZE 8388208)
set(STACK_SIZE 61800)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp -D__HEAP_SIZE=${HEAP_SIZE} -D__STACK_SIZE=${STACK_SIZE}")

set(MCFLAGS -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -D__FPU_USED=1)
set(CFLAGS ${MCFLAGS} -Wall -Wextra -falign-functions=16 -fomit-frame-pointer -fno-common -O2)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(GENERATED_LOADER_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/loader)
set(GENERATED_LOADER_START_INC ${GENERATED_LOADER_INCLUDE_DIR}/start.inc)

add_custom_command(
	OUTPUT ${GENERATED_LOADER_START_INC}
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_start.py ${GENERATED_LOADER_START_INC}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_start.py
	VERBATIM
)

add_custom_target(generateLoaderStartInc DEPENDS ${GENERATED_LOADER_START_INC})

# Rev. A loader

add_executable(loaderRevA src/loader/main.c)
add_dependencies(loaderRevA generateLoaderStartInc)
target_include_directories(loaderRevA PUBLIC include ${GENERATED_LOADER_INCLUDE_DIR})

target_compile_options(loaderRevA PRIVATE ${CFLAGS} -DTARGET_DVT1=1)

target_link_options(loaderRevA PUBLIC ${MCFLAGS})
target_link_options(loaderRevA PUBLIC -T${CMAKE_CURRENT_SOURCE_DIR}/ld/loader.ld -nostartfiles --entry playbrew_loader_start)
target_link_options(loaderRevA PUBLIC "-Wl,--gc-sections,--no-warn-mismatch")

add_custom_command(
	TARGET loaderRevA POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary loaderRevA loader-a.bin
)

set_property(
	TARGET loaderRevA
	PROPERTY ADDITIONAL_CLEAN_FILES
	loader-a.bin
)

# Rev. B loader

add_executable(loaderRevB src/loader/main.c)
add_dependencies(loaderRevB generateLoaderStartInc)
target_include_directories(loaderRevB PUBLIC include ${GENERATED_LOADER_INCLUDE_DIR})

target_compile_options(loaderRevB PRIVATE ${CFLAGS} -DTARGET_H7D1=1)

target_link_options(loaderRevB PUBLIC ${MCFLAGS})
target_link_options(loaderRevB PUBLIC -T${CMAKE_CURRENT_SOURCE_DIR}/ld/loader.ld -nostartfiles --entry playbrew_loader_start)
target_link_options(loaderRevB PUBLIC "-Wl,--gc-sections,--no-warn-mismatch")

add_custom_command(
	TARGET loaderRevB POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary loaderRevB loader-b.bin
)

set_property(
	TARGET loaderRevB
	PROPERTY ADDITIONAL_CLEAN_FILES
	loader-b.bin
)
