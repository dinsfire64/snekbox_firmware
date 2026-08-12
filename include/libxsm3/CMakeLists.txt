set(LIBXSM3_SOURCES
    excrypt_des.c
    excrypt_parve.c
    excrypt_sha.c
    usbdsec.c
    xsm3.c
)

add_library(libxsm3 STATIC ${LIBXSM3_SOURCES})

target_include_directories(libxsm3
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)