include_directories(.)

add_library(Chip8 STATIC chip.cpp chip.hpp keyboard.hpp)
target_compile_features(Chip8 PUBLIC cxx_std_17)

add_executable(Chip8_EXE main.cpp sfml_keyboard.cpp sfml_keyboard.hpp)

set_property(TARGET Chip8_EXE PROPERTY OUTPUT_NAME Chip8)

target_compile_features(Chip8_EXE PUBLIC cxx_std_17)

target_link_libraries(Chip8_EXE PRIVATE Chip8)
target_link_libraries(Chip8_EXE PRIVATE sfml-graphics)
target_link_libraries(Chip8_EXE PRIVATE sfml-window)
target_link_libraries(Chip8_EXE PRIVATE sfml-audio)
target_link_libraries(Chip8_EXE PRIVATE sfml-system)

if(WIN32)
    add_custom_command(
        TARGET Chip8_EXE
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Chip8_EXE>
        VERBATIM
    )
    add_custom_command(
        TARGET Chip8_EXE
        COMMENT "Copy SFML DLLs"
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-graphics> $<TARGET_FILE_DIR:Chip8_EXE>
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-window> $<TARGET_FILE_DIR:Chip8_EXE>
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-audio> $<TARGET_FILE_DIR:Chip8_EXE>
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sfml-system> $<TARGET_FILE_DIR:Chip8_EXE>
        VERBATIM
    )
endif()

configure_file(${PROJECT_SOURCE_DIR}/audio/beep.wav ${CMAKE_CURRENT_BINARY_DIR}/beep.wav COPYONLY)

add_subdirectory(tests)
