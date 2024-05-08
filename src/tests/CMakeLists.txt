include_directories(.)

# Chip::ProcessInstruction
add_executable(TestChipDisplay_EXE test_chip_display.cpp)
target_link_libraries(TestChipDisplay_EXE PRIVATE Chip8)
set_property(TARGET TestChipDisplay_EXE PROPERTY OUTPUT_NAME TestChipDisplay)
add_test(NAME TestChipDisplay COMMAND TestChipDisplay)
