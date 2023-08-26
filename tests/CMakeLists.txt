add_executable(Main main.cpp)

target_compile_features(Main PRIVATE cxx_std_17)

add_test(NAME MainTest COMMAND Main)

