# Install script for directory: D:/bch_sc2_cpp/main/contrib/SDL-mirror

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/cpp-sc2")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2maind.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2main.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2main.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/bch_sc2_cpp/main/build/bin/SDL2main.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake"
         "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/CMakeFiles/Export/cmake/SDL2Targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/CMakeFiles/Export/cmake/SDL2Targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/CMakeFiles/Export/cmake/SDL2Targets-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/CMakeFiles/Export/cmake/SDL2Targets-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/CMakeFiles/Export/cmake/SDL2Targets-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/CMakeFiles/Export/cmake/SDL2Targets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/SDL2Config.cmake"
    "D:/bch_sc2_cpp/main/build/SDL2ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_assert.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_atomic.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_audio.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_bits.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_blendmode.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_clipboard.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_android.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_iphoneos.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_macosx.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_minimal.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_os2.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_pandora.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_psp.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_windows.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_winrt.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_config_wiz.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_copying.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_cpuinfo.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_egl.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_endian.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_error.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_events.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_filesystem.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_gamecontroller.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_gesture.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_haptic.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_hints.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_joystick.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_keyboard.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_keycode.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_loadso.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_locale.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_log.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_main.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_messagebox.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_metal.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_misc.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_mouse.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_mutex.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_name.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengl.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengl_glext.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengles.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengles2.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengles2_gl2.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengles2_gl2ext.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengles2_gl2platform.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_opengles2_khrplatform.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_pixels.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_platform.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_power.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_quit.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_rect.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_render.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_revision.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_rwops.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_scancode.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_sensor.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_shape.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_stdinc.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_surface.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_system.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_syswm.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_assert.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_common.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_compare.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_crc32.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_font.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_fuzzer.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_harness.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_images.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_log.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_md5.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_memory.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_test_random.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_thread.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_timer.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_touch.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_types.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_version.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_video.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/SDL_vulkan.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/begin_code.h"
    "D:/bch_sc2_cpp/main/contrib/SDL-mirror/include/close_code.h"
    "D:/bch_sc2_cpp/main/build/contrib/SDL-mirror/include/SDL_config.h"
    )
endif()

