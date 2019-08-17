LIST(APPEND WTF_SOURCES
    generic/WorkQueueGeneric.cpp
    generic/MainThreadGeneric.cpp
    linux/MemoryPressureHandlerLinux.cpp

    posix/FileSystemPOSIX.cpp
    posix/ThreadingPOSIX.cpp

    unicode/icu/CollatorICU.cpp

    unix/CPUTimeUnix.cpp

    haiku/RunLoopHaiku.cpp
    haiku/LanguageHaiku.cpp
    haiku/CurrentProcessMemoryStatus.cpp
    haiku/MemoryFootprintHaiku.cpp
    haiku/OSAllocatorHaiku.cpp

    text/haiku/TextBreakIteratorInternalICUHaiku.cpp
)

LIST(APPEND WTF_LIBRARIES
    ${ZLIB_LIBRARIES}
    be execinfo
)

list(APPEND WTF_INCLUDE_DIRECTORIES
	/system/develop/headers/private/system/arch/$ENV{BE_HOST_CPU}/
	/system/develop/headers/private/system
)

add_definitions(-D_BSD_SOURCE=1)
