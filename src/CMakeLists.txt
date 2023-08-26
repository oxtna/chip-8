file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.hpp")

add_executable(Chip8_EXE main.cpp chip.cpp ${HEADER_LIST})

set_property(TARGET Chip8_EXE PROPERTY OUTPUT_NAME Chip8)

target_include_directories(Chip8_EXE PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(Chip8_EXE PUBLIC cxx_std_17)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

