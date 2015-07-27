add_subdirectory(platinum)
add_subdirectory(threadpool11)
add_subdirectory(crossguid)
add_subdirectory(pire)

find_package(Protobuf)

IF (NOT ${PROTOBUF_FOUND})
    SET (BUILD_TESTING OFF)
    add_subdirectory(protobuf/cmake)
ENDIF ()
