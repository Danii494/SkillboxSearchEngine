#include "InvertedIndex.h"
#include <sstream>
#include <thread>
#include <algorithm>

std::vector<std::string> InvertedIndex::splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    docs = input_docs;
    freq_dictionary.clear();

    std::vector<std::thread> threads;
    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        threads.emplace_back([this, doc_id]() {
            auto words = splitWords(docs[doc_id]);
            std::map<std::string, size_t> wordCount;
            for (const auto& w : words) {
                wordCount[w]++;
            }
            std::lock_guard<std::mutex> lock(dict_mutex);
            for (const auto& [word, cnt] : wordCount) {
                freq_dictionary[word].push_back({doc_id, cnt});
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    std::lock_guard<std::mutex> lock(dict_mutex);
    auto it = freq_dictionary.find(word);
    if (it != freq_dictionary.end())
        return it->second;
    return {};
}