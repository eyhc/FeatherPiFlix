#include <fstream>
#include <csv.h>
#include <cstring>
#include <functional>

#include "core/catalog.h"
#include "core/utils.h"

using namespace std;
using namespace core;

#define PAGE_SIZE 10

/*------------------------------------------
            CONSTRUCTORS HELPER
 -------------------------------------------*/

// Parse a CSV file and build FullMovie objects.
// For each row, call the provided function f(movie).
static void parse_csv(string filename, function<void(data::FullMovie)> f) {
    ifstream in(filename);

    // Column indexes (initialized to -1 before header parsing)
    bool is_first = true;
    int title_index = -1;
    int year_index = -1;
    int category_index = -1;
    int cover_normal_index = -1;
    int cover_square_index = -1;
    int director_index = -1;
    int producer_index = -1;
    int actors_index = -1;
    int synopsis_index = -1;
    int video_file_index = -1;
    int duration_index = -1;

    // Function called for each row
    csv::row_callback on_row = [&](vector<string> row) {
        // First row = header -> find column indexes
        if (is_first) {
            // locate each column by name
            auto itt  = find(row.begin(), row.end(), "title");
            auto ity  = find(row.begin(), row.end(), "year");
            auto itc  = find(row.begin(), row.end(), "category");
            auto itd  = find(row.begin(), row.end(), "director");
            auto itp  = find(row.begin(), row.end(), "producer");
            auto itdt = find(row.begin(), row.end(), "duration");
            auto ita  = find(row.begin(), row.end(), "actors");
            auto its  = find(row.begin(), row.end(), "synopsis");
            auto itf  = find(row.begin(), row.end(), "video_file");
            auto itnc = find(row.begin(), row.end(), "normal_cover");
            auto itsc = find(row.begin(), row.end(), "squared_cover");

            // if any column is missing, throw error
            if (itt != row.end()
                && ity  != row.end()
                && itc  != row.end()
                && itd  != row.end()
                && itp  != row.end()
                && itdt != row.end()
                && ita  != row.end()
                && its  != row.end()
                && itf  != row.end()
                && itnc  != row.end()
                && itsc  != row.end()
            ) {
                title_index        = distance(row.begin(), itt);
                year_index         = distance(row.begin(), ity);
                category_index     = distance(row.begin(), itc);
                director_index     = distance(row.begin(), itd);
                producer_index     = distance(row.begin(), itp);
                duration_index     = distance(row.begin(), itdt);
                actors_index       = distance(row.begin(), ita);
                synopsis_index     = distance(row.begin(), its);
                video_file_index   = distance(row.begin(), itf);
                cover_normal_index = distance(row.begin(), itnc);
                cover_square_index = distance(row.begin(), itsc);
            }
            else throw runtime_error("Missing column(s)");

            is_first = false;
        }

        // For each data row, create a FullMovie and call callback
        else  {
            f(data::FullMovie(
                row[title_index],
                atoi(row[year_index].c_str()),
                row[producer_index],
                row[category_index], 
                data::Cover(row[cover_normal_index], row[cover_square_index]),
                row[director_index],
                row[actors_index],
                row[synopsis_index],
                atoi(row[duration_index].c_str()),
                row[video_file_index]
            ));
        }
    };

    try {
        csv::read(in, on_row);
    }
    catch(const std::exception& e) { /* Ignore exception */ }

    in.close();
}


/*------------------------------------------
               BASIC CATALOG
 -------------------------------------------*/

BasicCatalog::BasicCatalog() = default;

BasicCatalog::BasicCatalog(const string& filename) {
    parse_csv(filename, [&](data::FullMovie m)->void {
            add(make_unique<data::FullMovie>(m));
        }
    );
}

void BasicCatalog::add(unique_ptr<data::Movie> m) {
    // checks first if already exist in this catalog
    if (!get_movie(m.get()->title()))
        _data.push_back(std::move(m));
}

void BasicCatalog::remove(const string &title) {
    size_t i = 0;
    while (i < _data.size() && _data[i].get()->title() != title) i++;
    
    if (i != _data.size())
        _data.erase(next(_data.begin(), i));
}

size_t BasicCatalog::size() const { return _data.size(); }

std::optional<size_t> BasicCatalog::get_index(const string &title) {
    for (size_t i = 0; i < _data.size(); i++) {
        if (_data[i].get()->title() == title) {
            return i;
        }
    }
    return nullopt;
}

data::Movie *BasicCatalog::get_movie(const size_t index) {
    return _data.at(index).get();
}

data::Movie *BasicCatalog::get_movie(const std::string& title) {
    optional<size_t> index = get_index(title);
    if (!index.has_value()) return nullptr;
    else return get_movie(index.value());
}

vector<data::Movie *> BasicCatalog::all_movies() {
    vector<data::Movie*> v;
    for (const auto &movie: _data) {
        v.push_back(movie.get());
    }
    return v;
}

void BasicCatalog::save(const string &filename) const {
    ofstream out(filename);

    // write headers
    csv::write_row(out, {
        "title",
        "year",
        "category",
        "director",
        "producer",
        "duration",
        "actors",
        "synopsis",
        "video_file",
        "normal_cover",
        "squared_cover"
    });

    // write all movies
    for (auto &mv: _data) {
        data::Movie *movie = mv.get();
        string synopsis = movie->synopsis().has_value() ?
                            movie->synopsis().value() : "";
        csv::write_row(out, {
            movie->title(),
            to_string(movie->year()),
            movie->category(),
            movie->director(),
            movie->producer(),
            to_string(movie->duration()),
            movie->actors(),
            synopsis,
            movie->video_file().generic_string(),
            movie->cover().normal_path().generic_string(),
            movie->cover().square_path().generic_string()
        });
    }

    out.close();
}


/*------------------------------------------
              CACHED CATALOG
 -------------------------------------------*/

CachedCatalog::CachedCatalog(size_t cache_size): _cache_size(cache_size) {
    _cache.reserve(cache_size);
}

CachedCatalog::CachedCatalog(const string &filename, size_t cache_size): 
    _cache_size(cache_size)
{
    _cache.reserve(cache_size);

    parse_csv(filename, [&](data::FullMovie m)->void {
            add(make_unique<data::LazyMovie>(
                m.title(),
                m.year(),
                m.producer(),
                m.category(),
                m.cover(),
                m.director(),
                m.actors(),
                filename,
                m.duration(),
                m.video_file()
            ));
        }
    );
}

void CachedCatalog::remove(const string &title) {
    BasicCatalog::remove(title);
    _cache.erase(title);
}

optional<string> CachedCatalog::get_synopsis_using_cache(const string &title) {
    auto it = _cache.find(title);

    // 1. return from cache if present and update usage order
    if (it != _cache.end()) {
        auto pos = find(_order.begin(), _order.end(), title);
        _order.erase(pos);
        _order.push_back(title);
        return it->second;
    }

    // 2. otherwise load from movie
    data::Movie *f = get_movie(title);
    if (!f) return nullopt;
    
    optional<string> s = f->synopsis();
    if (s.has_value()) {
        // 3. evict if cache full
        if (_cache.size() >= _cache_size) {
            auto old = _order.front();
            _order.erase(_order.begin());
            _cache.erase(old);
        }

        // 4. insert into cache
        _cache[title] = s.value();
        _order.push_back(title);
    }

    return s;
}

bool CachedCatalog::is_cached(const string &title) {
    auto it = _cache.find(title);
    return it != _cache.end();
}


/*------------------------------------------
           PAGED CACHED CATALOG
 -------------------------------------------*/

// PagedCachedCatalog caches whole pages of synopsis (page = 10 movies).
PagedCachedCatalog::PagedCachedCatalog(size_t cache_size): 
    _cache_size(cache_size) {}

PagedCachedCatalog::PagedCachedCatalog(
    const string &filename, size_t cache_size
): _cache_size(cache_size) {
    parse_csv(filename, [&](data::FullMovie m)->void {
            add(make_unique<data::LazyMovie>(
                m.title(),
                m.year(),
                m.producer(),
                m.category(),
                m.cover(),
                m.director(),
                m.actors(),
                filename,
                m.duration(),
                m.video_file()
            ));
        }
    );
}

bool PagedCachedCatalog::is_cached(const string &title) {
    optional<size_t> index = get_index(title);
    return index.has_value() && get_page(index.value() / PAGE_SIZE).has_value();
}

optional<string> PagedCachedCatalog::get_synopsis_using_cache(
    const string& title
) {
    optional<size_t> index = get_index(title);
    if (!index.has_value()) return nullopt;
    size_t i = index.value();

    // 1. try to get the page
    auto page = get_page(i / PAGE_SIZE);

    // 2. if page exists, search inside
    if (page.has_value()) {
        auto it = page->find(title);
        if (it != page->end()) return it->second;
    }

    // 3. load or reload page if needed
    load_page(i / PAGE_SIZE);

    // 4. try or try again to get the synopsis
    auto itpage = _cache.find(i / PAGE_SIZE);
    auto it = itpage->second.find(title);
    if (it != itpage->second.end()) {
        return it->second;
    }

    // 5. fallback: ask movie directly
    auto m = get_movie(title);
    return (m) ? m->synopsis() : nullopt; 
}

optional<unordered_map<string, string>> PagedCachedCatalog::get_page(size_t index) {
    auto itpage = _cache.find(index);
    if (itpage == _cache.end()) return nullopt;

    // update LRU order
    auto pos = find(_order.begin(), _order.end(), index);
    _order.erase(pos);
    _order.push_back(index);

    return itpage->second;
}

void PagedCachedCatalog::load_page(size_t index) {
    // 1. erase existing entry if present
    auto it = _cache.find(index);
    if (it != _cache.end()) {
        _cache.erase(it);
        auto itorder = find(_order.begin(), _order.end(), index);
        _order.erase(itorder);
    }

    // 2. prepare temporary page (only titles of movies)
    size_t last_index = min(PAGE_SIZE * (index + 1), size());
    unordered_map<string, optional<string>> temp_page;
    for (size_t i = PAGE_SIZE * index; i < last_index; i++) {
        temp_page[get_movie(i)->title()] = nullopt;
    }
    
    // 3. parse CSV to fill synopses
    parse_csv(filename, [&temp_page](data::FullMovie m) -> void {
        if (temp_page.count(m.title())) {
            temp_page[m.title()] = m.synopsis();
        }
    });

    // 4. fallback: ask Movie objects directly if missing
    for (size_t i = PAGE_SIZE * index; i < last_index; i++) {
        auto *m = get_movie(i);
        if (temp_page[m->title()] == nullopt)
            temp_page[m->title()] = m->synopsis();
    }

    // 5. evict oldest page if cache is full
    if (_cache.size() >= _cache_size) {
        size_t old = _order.front();
        _order.erase(_order.begin());
        _cache.erase(old);
    }

    // 6. insert new page in cache
    unordered_map<string, string> page;
    for (auto i = temp_page.begin(); i != temp_page.end(); i++) {
        page[i->first] = (i->second.has_value()) ? i->second.value() : "";
    }
    _cache[index] = page;
    _order.push_back(index);
}
