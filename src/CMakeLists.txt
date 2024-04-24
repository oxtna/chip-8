file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.hpp")

add_executable(Chip8_EXE main.cpp chip.cpp ${HEADER_LIST})

set_property(TARGET Chip8_EXE PROPERTY OUTPUT_NAME Chip8)

target_include_directories(Chip8_EXE PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_features(Chip8_EXE PUBLIC cxx_std_17)

target_link_libraries(Chip8_EXE PRIVATE sfml-graphics)
target_link_libraries(Chip8_EXE PRIVATE sfml-window)
target_link_libraries(Chip8_EXE PRIVATE sfml-system)

source_group(
    TREE ${PROJECT_SOURCE_DIR}
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

if(WIN32)
    add_custom_command(
        TARGET Chip8_EXE
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<BOOL:${ARCH_64BITS}>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Chip8_EXE>
        VERBATIM
    )
    add_custom_command(
        TARGET Chip8_EXE
        COMMENT "Copy SFML DLLs"
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-graphics> $<TARGET_FILE_DIR:Chip8_EXE>
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-window> $<TARGET_FILE_DIR:Chip8_EXE>
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-system> $<TARGET_FILE_DIR:Chip8_EXE>
        VERBATIM
    )
endif()
