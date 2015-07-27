project(crossguid)

if ( UNIX )
    add_definitions(-DGUID_LIBUUID)
    find_library( UUID_LIBS NAMES uuid )
elseif ( WIN32 )
    add_definitions(-DGUID_WINDOWS)
    set( UUID_LIBS  )
endif ()

set(EPS_SRCS
  guid.cpp
  guid.h
)

include_directories(${CMAKE_BINARY_DIR})

add_library(crossguid
  ${EPS_SRCS}
)

target_link_libraries(crossguid
  ${UUID_LIBS}
)
