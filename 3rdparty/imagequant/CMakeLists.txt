add_library(quant STATIC IMPORTED GLOBAL)

if(TARGET quant)
    message(STATUS "quant exists inside imagequant")
else()
    message(FATAL_ERROR "quant does NOT exist inside imagequant")
endif()

if(WIN32)
    set(IMAGEQUANT_LIB quant_w.a)
else()
    set(IMAGEQUANT_LIB quant.a)
endif()

set_target_properties(quant PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${IMAGEQUANT_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)