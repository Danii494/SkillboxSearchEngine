#include "ConverterJSON.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

void ConverterJSON::checkConfig() const {
    if (!fs::exists("config.json"))
        throw std::runtime_error("config file is missing");
    std::ifstream f("config.json");
    json data = json::parse(f);
    if (!data.contains("config"))
        throw std::runtime_error("config file is empty");
}

void ConverterJSON::checkConfigVersion() const {
    std::ifstream f("config.json");
    json data = json::parse(f);
    std::string version = data["config"]["version"];
    if (version != "0.1")
        throw std::runtime_error("config.json has incorrect file version");
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    checkConfig();
    checkConfigVersion();
    
    std::ifstream f("config.json");
    if (!f.is_open()) {
        throw std::runtime_error("Could not open config.json");
    }
    
    json data = json::parse(f);
    std::vector<std::string> filePaths = data["files"];
    std::vector<std::string> texts;
    
    for (const auto& path : filePaths) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Warning: cannot open file " << path << ", skipping.\n";
            continue;
        }
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        texts.push_back(content);
    }
    return texts;
}

int ConverterJSON::GetResponsesLimit() {
    std::ifstream f("config.json");
    if (!f.is_open()) {
        throw std::runtime_error("Could not open config.json");
    }
    json data = json::parse(f);
    if (data["config"].contains("max_responses"))
        return data["config"]["max_responses"];
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::ifstream f("requests.json");
    if (!f.is_open()) {
        throw std::runtime_error("requests.json could not be opened");
    }
    json data = json::parse(f);
    return data["requests"];
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    std::ofstream out("answers.json");
    if (!out.is_open()) {
        throw std::runtime_error("Could not open answers.json for writing");
    }
    
    json output;
    output["answers"] = json::object();
    int reqIndex = 1;
    for (const auto& ans : answers) {
        std::string reqId = "request" + 
            std::string(3 - std::to_string(reqIndex).length(), '0') + 
            std::to_string(reqIndex);
        if (ans.empty()) {
            output["answers"][reqId] = {{"result", "false"}};
        } else {
            json relevance = json::array();
            for (const auto& [docid, rank] : ans) {
                relevance.push_back({{"docid", docid}, {"rank", rank}});
            }
            output["answers"][reqId] = {{"result", "true"}, {"relevance", relevance}};
        }
        ++reqIndex;
    }
    out << output.dump(4);
}