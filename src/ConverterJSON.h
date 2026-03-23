#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class ConverterJSON {
public:
    ConverterJSON() = default;

    std::vector<std::string> GetTextDocuments();

    int GetResponsesLimit();

    std::vector<std::string> GetRequests();

    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    void checkConfig() const;
    void checkConfigVersion() const;
};