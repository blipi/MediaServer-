add_subdirectory(platinum)
add_subdirectory(threadpool11)
add_subdirectory(crossguid)

add_custom_target(
   ohnet
   COMMAND make native_only=yes
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/dep/ohnet
)

find_library( OHNET_LIBRARIES_CORE
    NAMES ohNetCore
    PATHS ${CMAKE_SOURCE_DIR}/dep/ohnet/Build/Obj/Posix/Release
    NO_DEFAULT_PATH)

find_library( OHNET_LIBRARIES_DEVICES
    NAMES ohNetDevices
    PATHS ${CMAKE_SOURCE_DIR}/dep/ohnet/Build/Obj/Posix/Release
    NO_DEFAULT_PATH)

set ( OHNET_LIBRARIES
        ${OHNET_LIBRARIES_CORE}
        ${OHNET_LIBRARIES_DEVICES})

if ( UNIX )
elseif ( WIN32 )

endif ()
