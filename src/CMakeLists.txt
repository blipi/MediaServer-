# Based on TrinityCore CMake files
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Main Main/*.cpp     Main/*.hpp)

set(EPS_SRCS
  ${EPS_SRCS}
  ${sources_Main}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Main

  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Platinum
  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Core
  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Devices/MediaServer
  ${CMAKE_SOURCE_DIR}/dep/platinum/Neptune/Source/Core
)

add_executable(MediaServerPP
  ${EPS_SRCS}
)

target_link_libraries(MediaServerPP
  ${DEPENDENCIES}
  upnp
)
