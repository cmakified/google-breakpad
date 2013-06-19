CMAKE_MINIMUM_REQUIRED (VERSION 2.6.0)
PROJECT (google-breakpad)

SET (CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
SET (CMAKE_INCLUDE_CURRENT_DIR ON)

INCLUDE_DIRECTORIES(
	src/
)

SET (SOURCES
	src/common/md5.cc)

if (UNIX) 
	ADD_DEFINITIONS(-Wall)
	LIST (APPEND SOURCES
		src/client/minidump_file_writer.cc
		src/common/convert_UTF.c
		src/common/string_conversion.cc
	)
ENDIF ()

if (WIN32) 
	LIST (APPEND SOURCES
		src/common/windows/guid_string.cc
		src/client/windows/crash_generation/crash_generation_client.cc
		src/client/windows/crash_generation/client_info.cc
		src/client/windows/crash_generation/minidump_generator.cc
		src/client/windows/handler/exception_handler.cc
	)
	ADD_DEFINITIONS(/GL /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_UNICODE" /D "UNICODE" /EHsc )
	SET(EXTRA_LIBS ole32)
endif ()

if (UNIX)
	if (APPLE)
		INCLUDE_DIRECTORIES(/Developer/Headers/FlatCarbon)
		FIND_LIBRARY(COCOA_LIBRARY COCOA)
		MARK_AS_ADVANCED(COCOA_LIBRARY)
		SET(EXTRA_LIBS crypto ${COCOA_LIBRARY})
		LIST (APPEND SOURCES
			src/client/mac/handler/breakpad_nlist_64.cc
			src/client/mac/handler/dynamic_images.cc
			src/client/mac/handler/exception_handler.cc
			src/client/mac/handler/minidump_generator.cc
			src/client/mac/handler/protected_memory_allocator.cc
			src/client/mac/crash_generation/crash_generation_server.cc
			src/client/mac/crash_generation/crash_generation_client.cc
			src/common/mac/dump_syms.mm
			src/common/dwarf/bytereader.cc
			src/common/dwarf/dwarf2reader.cc
			src/common/dwarf/functioninfo.cc
			src/common/mac/file_id.cc
			src/common/mac/macho_id.cc
			src/common/mac/macho_utilities.cc
			src/common/mac/macho_walker.cc
			src/common/mac/string_utilities.cc
			src/common/mac/MachIPC.mm
			src/common/mac/bootstrap_compat.cc
		)
	else ()
		LIST (APPEND SOURCES
			src/client/linux/handler/exception_handler.cc
			src/client/linux/handler/linux_thread.cc
			src/client/linux/handler/minidump_generator.cc
			src/common/linux/dump_symbols.cc
			src/common/linux/file_id.cc
			src/common/linux/guid_creator.cc
			src/common/linux/http_upload.cc
		)
	endif ()
endif ()

IF(UNIX AND NOT APPLE)
	SET_TARGET_PROPERTIES (breakpad PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF ()

ADD_LIBRARY (breakpad ${SOURCES})
TARGET_LINK_LIBRARIES (breakpad ${EXTRA_LIBS})

