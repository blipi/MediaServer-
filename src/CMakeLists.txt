# Based on TrinityCore CMake files
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Main      Main/*.cpp      Main/*.h)
file(GLOB_RECURSE sources_Server    Server/*.cpp    Server/*.h)
file(GLOB_RECURSE sources_Network   Network/*.cpp   Network/*.hpp)
file(GLOB_RECURSE sources_Renderers Renderers/*.cpp Renderers/*.hpp)

set(EPS_SRCS
  ${EPS_SRCS}
  ${sources_Main}
  ${sources_Server}
  ${sources_Network}
  ${sources_Renderers}
)

find_package(Protobuf)
IF (NOT ${PROTOBUF_FOUND})
    FIND_LIBRARY(PROTOBUF_LIBRARY
        NAMES protobuf libprotobuf
        PATHS ${CMAKE_BINARY_DIR}/dep/protobuf
        PATH_SUFFIXES
            cmake
            cmake/debug
            cmake/release
            cmake/Debug
            cmake/Release
        NO_DEFAULT_PATH
    )

    FIND_LIBRARY(PROTOBUF_PROTOC_LIBRARY
        NAMES protoc libprotoc
        PATHS ${CMAKE_BINARY_DIR}/dep/protobuf
        PATH_SUFFIXES
            cmake
            cmake/debug
            cmake/release
            cmake/Debug
            cmake/Release
        NO_DEFAULT_PATH
    )

    FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE
        NAMES protoc
        PATHS ${CMAKE_BINARY_DIR}/dep/protobuf
        PATH_SUFFIXES
            cmake
            cmake/debug
            cmake/release
            cmake/Debug
            cmake/Release
        NO_DEFAULT_PATH
    )

    IF (PROTOBUF_LIBRARY AND PROTOBUF_PROTOC_LIBRARY AND PROTOBUF_PROTOC_EXECUTABLE)
        SET (PROTOBUF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/protobuf/src)
    ELSE ()
        MESSAGE (FATAL_ERROR "Protobuf not found")
    ENDIF ()

ENDIF ()

IF (${BUILD_PROTO_MESSAGES})
    PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${CMAKE_SOURCE_DIR}/resources/proto/Renderer.proto")
ELSE ()
    SET (PROTO_SRCS
        ${CMAKE_SOURCE_DIR}/resources/compiled/Renderer.pb.cc
    )
    SET (PROTOBUF_INCLUDE_DIR ${PROTOBUF_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/resources/compiled)
ENDIF ()


include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Main
  ${CMAKE_CURRENT_SOURCE_DIR}/Server
  ${CMAKE_CURRENT_SOURCE_DIR}/Network
  ${CMAKE_CURRENT_SOURCE_DIR}/Renderers

  ${PROTOBUF_INCLUDE_DIR}

  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Core
  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Platinum
  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Devices/MediaConnect
  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Devices/MediaRenderer
  ${CMAKE_SOURCE_DIR}/dep/platinum/Platinum/Source/Devices/MediaServer
  ${CMAKE_SOURCE_DIR}/dep/platinum/Neptune/Source/Core

  ${CMAKE_SOURCE_DIR}/dep/pire
  ${CMAKE_BINARY_DIR}/dep/pire
  ${CMAKE_SOURCE_DIR}/dep/crossguid
  ${CMAKE_SOURCE_DIR}/dep/threadpool11/threadpool11/include
)

add_executable(MediaServerPP
  ${EPS_SRCS}
  ${PROTO_SRCS}
)

target_link_libraries(MediaServerPP
  ${DEPENDENCIES}
  ${UUID_LIBS}
  ${PROTOBUF_LIBRARY}
  crossguid
  threadpool11
  upnp
  pire
)

macro(copy_dirs GLOBPAT DESTINATION)
  file(GLOB COPY_DIRS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBPAT})
  add_custom_target(resources ALL
    DEPENDS ${GLOBPAT}

    COMMENT "Copying files: ${COPY_DIRS}")

  foreach(FILENAME ${COPY_DIRS})
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${DESTINATION}")

    if (UNIX)
        add_custom_command(
          TARGET resources
          COMMAND ${CMAKE_COMMAND} -E remove ${DST}
    		COMMENT "Remove ${DST}"
        )

        add_custom_command(
          TARGET resources
          COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC} ${DST}
          COMMENT "copy_directory ${SRC} ${DST}"
        )
    else (UNIX)
        add_custom_command(
          TARGET resources
          COMMAND ${CMAKE_COMMAND} -E remove_directory ${DST}
    		COMMENT "Remove ${DST}"
        )

        add_custom_command(
          TARGET resources
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${SRC} ${DST}
    	  COMMENT "copy_directory ${SRC} ${DST}"
        )
    endif (UNIX)

    add_dependencies(resources MediaServerPP)
  endforeach(FILENAME)
endmacro(copy_dirs)

copy_dirs(${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:MediaServerPP>/resources)
