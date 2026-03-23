#include "SearchServer.h"
#include <sstream>
#include <algorithm>
#include <set>
#include <map>

std::vector<std::string> SearchServer::getUniqueSortedWords(const std::string& query) {
    std::vector<std::string> words;
    std::istringstream iss(query);
    std::string w;
    while (iss >> w) {
        words.push_back(w);
    }
    std::set<std::string> unique(words.begin(), words.end());
    std::vector<std::string> sorted(unique.begin(), unique.end());
    std::sort(sorted.begin(), sorted.end(),
        [this](const std::string& a, const std::string& b) {
            return _index.GetWordCount(a).size() < _index.GetWordCount(b).size();
        });
    return sorted;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> results;
    for (const auto& query : queries_input) {
        if (query.empty()) {
            results.emplace_back();
            continue;
        }

        auto words = getUniqueSortedWords(query);
        if (words.empty()) {
            results.emplace_back();
            continue;
        }

        std::map<size_t, size_t> docAbsolute;
        for (const auto& word : words) {
            auto entries = _index.GetWordCount(word);
            for (const auto& e : entries) {
                docAbsolute[e.doc_id] += e.count;
            }
        }

        if (docAbsolute.empty()) {
            results.emplace_back();
            continue;
        }

        float maxAbs = 0;
        for (const auto& [_, abs] : docAbsolute) {
            if (abs > maxAbs) maxAbs = static_cast<float>(abs);
        }

        std::vector<RelativeIndex> rel;
        for (const auto& [doc_id, abs] : docAbsolute) {
            rel.push_back({doc_id, abs / maxAbs});
        }

        std::sort(rel.begin(), rel.end(),
            [](const RelativeIndex& a, const RelativeIndex& b) {
                if (a.rank != b.rank) return a.rank > b.rank;
                return a.doc_id < b.doc_id;
            });

        if (rel.size() > maxResponses)
            rel.resize(maxResponses);

        results.push_back(rel);
    }
    return results;
}