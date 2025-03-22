cmake_minimum_required(VERSION 3.14)

if (NOT DEFINED ENV{VITASDK} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Please define VITASDK env var to point to your SDK path or set CMAKE_TOOLCHAIN_FILE.")
endif ()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "Toolchain file")

project(melange_vita C CXX)

include("${VITASDK}/share/vita.cmake" REQUIRED)

# Vita application configuration
set(VITA_APP_NAME "MelangeVita")
set(VITA_TITLEID "MELANGE00")
set(VITA_VPKNAME "melange_vita")
set(VITA_VERSION "01.00")
set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")

# Optional: used for "send" and "dump" targets
set(PSVITAIP "192.168.0.198" CACHE STRING "PSVita IP (for FTP access)")
set(PARSECOREPATH "$ENV{PARSECOREPATH}" CACHE STRING "vita-parse-core main.py")

# Paths for APK libraries
set(APK_LIB_PATH "${CMAKE_SOURCE_DIR}/apk_libs")
set(BREWEMU_SO "${APK_LIB_PATH}/libbrewemu.so")

# Ensure the required .so file exists
if (NOT EXISTS ${BREWEMU_SO})
  message(FATAL_ERROR "Missing libbrewemu.so! Extract it from the APK and place it in ${APK_LIB_PATH}")
endif()

# Path where Vita will look for the .so file
set(SO_PATH "ux0:data/melange/libbrewemu.so" CACHE STRING "Path to .so")
add_definitions(-DSO_PATH=\"${SO_PATH}\")

# Define debug mode if building in Debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG_MELANGE)
endif()

# Required definitions for some functions
add_definitions(-D_GNU_SOURCE -D__POSIX_VISIBLE=999999)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -O3 -g -ffast-math -mfloat-abi=softfp -Wno-deprecated")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++20 -Wno-write-strings -Wno-psabi")

# Add executable
add_executable(${CMAKE_PROJECT_NAME}
               source/main.c
               source/dynlib.c
               source/utils/logger.c
               source/utils/utils.c
               source/utils/dialog.c
               source/utils/init.c
               source/reimpl/io.c
               source/reimpl/sys.c
               source/reimpl/mem.c
               source/reimpl/log.c
               source/reimpl/errno.c
               lib/so_util/so_util.c
)

# Ensure libbrewemu.so is copied to the final package
install(FILES ${BREWEMU_SO} DESTINATION ux0:data/melange)

# Include directories
target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/source)

# Link Vita libraries
target_link_libraries(${CMAKE_PROJECT_NAME} 
                      kubridge_stub   # For dynamic .so loading
                      vitaGL
                      taihen_stub
                      SceLibKernel_stub
                      SceKernelDmacMgr_stub
                      SceAppMgr_stub
                      SceSysmodule_stub
                      m
                      z
)

# Vita output files
vita_create_self(eboot.bin ${CMAKE_PROJECT_NAME} UNSAFE NOASLR)
vita_create_vpk(${VITA_VPKNAME}.vpk ${VITA_TITLEID} eboot.bin
                VERSION ${VITA_VERSION}
                NAME ${VITA_APP_NAME}
                FILE ${CMAKE_SOURCE_DIR}/extras/icon0.png sce_sys/icon0.png
                ${CMAKE_SOURCE_DIR}/extras/livearea/bg0.png sce_sys/livearea/contents/bg0.png
                ${CMAKE_SOURCE_DIR}/extras/livearea/template.xml sce_sys/livearea/contents/template.xml
)

# Commands to send files to the Vita
add_custom_target(send
                  COMMAND echo destroy | nc ${PSVITAIP} 1338
                  COMMAND echo destroy | nc ${PSVITAIP} 1338
                  COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
                  COMMAND echo launch ${VITA_TITLEID} | nc ${PSVITAIP} 1338
                  DEPENDS ${VITA_VPKNAME}.vpk
)

add_custom_target(send_lib
                  COMMAND curl -T ${BREWEMU_SO} ftp://${PSVITAIP}:1337/ux0:/data/melange/
)

add_custom_target(dump
                  COMMAND ${CMAKE_SOURCE_DIR}/extras/scripts/get_dump.sh ${PSVITAIP}
                  COMMAND python3 ${PARSECOREPATH} coredump ${CMAKE_PROJECT_NAME}
)

add_custom_target(reboot
                  COMMAND echo reboot | nc ${PSVITAIP} 1338
)
