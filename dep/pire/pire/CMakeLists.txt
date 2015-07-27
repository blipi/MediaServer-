find_package(BISON REQUIRED)

IF (CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_definitions("-DPIRE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES DEBUG)

IF (${ENABLE_CHECKED})
    add_definitions("-DPIRE_CHECKED")
ENDIF ()

SET (libpire_la_SOURCES
	align.h
	any.h
	classes.cpp
	defs.h
	determine.h
	easy.cpp
	easy.h
	encoding.cpp
	encoding.h
	extra.h
	fsm.cpp
	fsm.h
	fwd.h
	glue.h
	partition.h
	pire.h
	re_lexer.cpp
	re_lexer.h
	run.h
	scanner_io.cpp
	static_assert.h
	platform.h
	vbitset.h
	re_parser.cpp
	scanners/loaded.h
	scanners/multi.h
	scanners/slow.h
	scanners/simple.h
	scanners/common.h
	scanners/pair.h
	scanners/null.cpp
	stub/stl.h
	stub/lexical_cast.h
	stub/saveload.h
	stub/singleton.h
	stub/utf8.cpp
	stub/utf8.h
	stub/noncopyable.h
	stub/codepage_h.h
	stub/doccodes_h.h
	stub/unidata_h.h
	stub/unidata_cpp.h)

IF (${ENABLE_EXTRA})
    SET (libpire_la_SOURCES
    ${libpire_la_SOURCES}
	extra/capture.cpp
	extra/capture.h
	extra/count.cpp
	extra/count.h
	extra/glyphs.cpp
	extra/glyphs.h)
ENDIF ()

SET (pire_include ${CMAKE_CURRENT_SOURCE_DIR})
SET (pire_hdr_HEADERS
	align.h
	any.h
	defs.h
	determine.h
	easy.h
	encoding.h
	extra.h
	fsm.h
	fwd.h
	glue.h
	partition.h
	pire.h
	re_lexer.h
	re_parser.h
	run.h
	static_assert.h
	platform.h
	vbitset.h)

SET (nodist_pire_hdr_HEADERS config.h)

IF (${ENABLE_EXTRA})
    SET (pire_include ${pire_include} ${CMAKE_CURRENT_SOURCE_DIR}/extra)

    SET (pire_extra_HEADERS
	extra/capture.h
	extra/count.h
	extra/glyphs.h)
ELSE ()
    SET (pire_extra_HEADERS )
ENDIF ()

SET (pire_include ${pire_include} ${CMAKE_CURRENT_SOURCE_DIR}/scanners)
SET (pire_scanners_HEADERS
	scanners/common.h
	scanners/multi.h
	scanners/slow.h
	scanners/simple.h
	scanners/loaded.h
	scanners/pair.h)

SET (pire_include ${pire_include} ${CMAKE_CURRENT_SOURCE_DIR}/stub)
SET (pire_stub_HEADERS
	stub/stl.h
	stub/defaults.h
	stub/memstreams.h
	stub/singleton.h
	stub/saveload.h
	stub/lexical_cast.h)

SET (pire_inline_SOURCES inline.lpp stub/hacks.h stub/memstreams.h)
SET (pire_inline_LDADD libpire.la)

SET (BUILT_SOURCES re_parser.h re_parser.cpp)
SET (CLEANFILES    re_parser.h re_parser.cpp)

BISON_TARGET(RE_PARSER
    "${CMAKE_CURRENT_SOURCE_DIR}/re_parser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/re_parser.cpp"
    COMPILE_FLAGS "--defines=re_parser.h")

MESSAGE ("Bisson:\n\t${BISON_RE_PARSER_OUTPUTS}")
MESSAGE ("\tSuccess: ${BISON_RE_PARSER_DEFINED}")

get_filename_component(BISON_INCLUDE ${BISON_RE_PARSER_OUTPUT_HEADER} DIRECTORY)
#FILE (COPY ${BISON_RE_PARSER_OUTPUT_HEADER}
#    DESTINATION ${BISON_INCLUDE}/re_parser.h)

include_directories(${pire_include}
    ${CMAKE_CURRENT_BINARY_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    ${BISON_INCLUDE})

if (MSVC)
    add_definitions(-D_XKEYCHECK_H)
endif ()

add_library(pire
    ${libpire_la_SOURCES}
    ${pire_inline_SOURCES}
    )
