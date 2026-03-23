add_executable(run_tests
    test.cpp
    ../src/ConverterJSON.cpp
    ../src/InvertedIndex.cpp
    ../src/SearchServer.cpp
)

target_link_libraries(run_tests PRIVATE gtest_main nlohmann_json::nlohmann_json)

add_test(NAME run_tests COMMAND run_tests)