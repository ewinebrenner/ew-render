string(TIMESTAMP BEFORE "%s")

CPMAddPackage(
        NAME "freetype"
        URL "https://gitlab.freedesktop.org/freetype/freetype/-/archive/VER-2-13-0/freetype-VER-2-13-0.zip"
        OPTIONS "BUILD_SHARED_LIBS ON" "FT_REQUIRE_ZLIB OFF" "FT_DISABLE_ZLIB OFF" "FT_DISABLE_BZIP2 ON" "FT_DISABLE_PNG ON" "FT_DISABLE_HARFBUZZ ON" "FT_DISABLE_BROTLI ON"
    )

    find_package(freetype REQUIRED)

    set (freetype_INCLUDE_DIR ${freetype_SOURCE_DIR}/include)
    include_directories(${freetype_INCLUDE_DIR})

    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTAfreetype "${AFTER} - ${BEFORE}")
    MESSAGE(STATUS "freetype TIME: ${DELTAfreetype}s")