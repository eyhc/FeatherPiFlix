#include "core/search.h"
#include "../utils.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace core;

int main(void) {
    search::Indexer *index = new search::Indexer("./index_db", "french");

    auto movies = test::create_collection();
    for (auto &m: movies)
        index->add(ref(*m.get()));

    assert(index->nb_movies() == 6);
    size_t nb_terms = index->nb_terms();
    assert(nb_terms > 20);

    data::Movie &m = *movies[5];
    auto m_ref = std::ref(m);

    string ancien = m.title();
    m.set_category("Fantastique");
    
    index->edit(ancien, m_ref);
    assert(index->nb_movies() == 6);
    assert(index->nb_terms() >= nb_terms + 1);
    
    data::Movie &m5 = *movies[5];
    m_ref = std::ref(m5);
    index->remove(m5.title());
    assert(index->nb_movies() == 5);
    nb_terms = index->nb_terms();
    delete index;

    index = new search::Indexer("./index_db", "french");
    assert(index->nb_movies() == 5);
    assert(index->nb_terms() == nb_terms);

    index->add(m_ref);
    assert(index->nb_movies() == 6);

    auto res = index->search("Victor Francen");
    assert(res.size() == 2);
    assert(res[0].first == "J'accuse" || res[0].first == "La Fin du jour");
    assert(res[1].first == "J'accuse" || res[1].first == "La Fin du jour");
    assert(res[0].second > 1 && res[1].second > 1);

    res = index->search("1993");
    assert(res.size() == 1);
    assert(res[0].first == "Germinal");
    assert(res[0].second > 1);

    res = index->search("jour");
    assert(res.size() == 2);
    assert(res[0].first == "La Fin du jour");
    assert(res[1].first == "La Trilogie Marseillaise : Fanny");

    index->clear();
    assert(index->nb_movies() == 0);
    assert(index->nb_terms() == 0);

    delete index;
    filesystem::remove_all("./index_db");

    cout << "TEST_SEARCH: OK" << endl;
    return 0;
}
