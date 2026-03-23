#pragma once
#include "InvertedIndex.h"
#include <vector>
#include <string>

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return doc_id == other.doc_id && std::abs(rank - other.rank) < 1e-6;
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx, int max_resp = 5) : _index(idx), maxResponses(max_resp) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;
    int maxResponses;

    std::vector<std::string> getUniqueSortedWords(const std::string& query);
};