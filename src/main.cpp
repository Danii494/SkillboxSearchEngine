#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <iostream>

int main() {
    try {
        ConverterJSON converter;
        std::vector<std::string> docs = converter.GetTextDocuments();
        if (docs.empty()) {
            std::cerr << "No documents to index. Exiting.\n";
            return 1;
        }

        InvertedIndex idx;
        idx.UpdateDocumentBase(docs);

        std::vector<std::string> requests = converter.GetRequests();
        if (requests.empty()) {
            std::cerr << "No requests to process.\n";
            return 0;
        }

        int limit = converter.GetResponsesLimit();
        SearchServer srv(idx, limit);

        auto rawResults = srv.search(requests);

        std::vector<std::vector<std::pair<int, float>>> answers;
        for (const auto& raw : rawResults) {
            std::vector<std::pair<int, float>> limited;
            for (const auto& rel : raw) {
                limited.emplace_back(rel.doc_id, rel.rank);
            }
            answers.push_back(limited);
        }

        converter.putAnswers(answers);
        std::cout << "Search completed. Results written to answers.json\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}