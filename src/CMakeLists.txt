add_executable(search_engine
    main.cpp
    ConverterJSON.cpp
    InvertedIndex.cpp
    SearchServer.cpp
)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

install(TARGETS search_engine DESTINATION bin)