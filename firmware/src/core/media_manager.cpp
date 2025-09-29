#include "core/media_manager.h"

using namespace std;
using namespace core;


MediaManager::MediaManager(
    const filesystem::path &index_database,
    const std::filesystem::path &movies_csv_file,
    const std::string &lang,
    cache_type catalog_type,
    size_t cache_size
) {
    _index = new search::Indexer(index_database, lang);

    // create CSV file if not exist.
    if (!filesystem::exists(movies_csv_file)) {
        BasicCatalog c;
        c.save(movies_csv_file);
    }

    if (catalog_type == INDIVIDUAL_CACHE)
        _movies = new CachedCatalog(movies_csv_file, cache_size);
    else if (catalog_type == PAGED_CACHE)
        _movies = new PagedCachedCatalog(movies_csv_file, cache_size);
    else
        _movies = new BasicCatalog(movies_csv_file);

    _csv_file = movies_csv_file;
}

MediaManager::~MediaManager() {
    flush();
    delete _movies;
    delete _index;
}

bool MediaManager::exists(const string &title) const {
    return _movies->exists(title);
}

optional<data::movie_ref> MediaManager::get_movie(const string &title) const {
    return _movies->get_movie(title);
}

vector<data::movie_ref> MediaManager::movies() const {
    return _movies->all_movies();
}

vector<data::movie_ref> MediaManager::movies(
    size_t offset, size_t count
) const {
    return _movies->movies_slice(offset, count);
}

size_t MediaManager::nb_movies() const {
    return _movies->size();
}

void MediaManager::add(unique_ptr<data::Movie> m) {
    _index->add(*m);
    _movies->add(move(m));
}

void MediaManager::remove(const string &title) {
    _index->remove(title);
    _movies->remove(title);
}

void MediaManager::reindex(const string &title) {
    auto m = _movies->get_movie(title);
    if (m.has_value())
        _index->edit(title, m.value());
}

void MediaManager::reindex_all() {
    _index->clear();
    for (auto &m: _movies->all_movies())
        _index->add(m);
}

void MediaManager::flush() {
    _movies->save(_csv_file);
    _index->flush();
}

vector<pair<data::movie_ref, double>> MediaManager::search(
    string query, size_t max_result
) const {
    auto result = _index->search(query, max_result);
    vector<pair<data::movie_ref, double>> vres;
    vres.reserve(result.size());

    for (auto &r: result)
        vres.push_back({_movies->get_movie(r.first).value(), r.second});
    
    return vres;
}
