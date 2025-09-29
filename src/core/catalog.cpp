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
           CACHED SYNOPSIS PROVIDER
  ------------------------------------------*/

CachedSynopsisProvider::CachedSynopsisProvider(
    const string &title,
    unique_ptr<data::SynopsisProvider> base_provider,
    function<optional<string>(const string&)> get_synopsis_using_cache
) {
    _title = title;
    _get_synopsis_using_cache = get_synopsis_using_cache;
    _base_provider = move(base_provider);
}

string CachedSynopsisProvider::get_synopsis() const {
    const auto res = _get_synopsis_using_cache(_title);
    return 
        (res.has_value()) ? res.value() : _base_provider.get()->get_synopsis();
}

void CachedSynopsisProvider::set_synopsis(const string &synopsis) {
    return _base_provider.get()->set_synopsis(synopsis);
}

reference_wrapper<data::SynopsisProvider>
    CachedSynopsisProvider::get_base_provider() const
{
    return ref(*_base_provider);
}


/*------------------------------------------
            CONSTRUCTORS HELPER
 -------------------------------------------*/

// Parse a CSV file and build FullMovie objects.
// For each row, call the provided function f(movie).
static void parse_csv(string filename, function<void(data::Movie&)> f) {
    ifstream in(filename);
    if (!in.is_open()) throw runtime_error("Cannot open file: " + filename);

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
            try {
                data::Movie m = data::Movie(
                    row[title_index],
                    atoi(row[year_index].c_str()),
                    row[category_index],
                    row[producer_index],
                    row[director_index],
                    row[actors_index],
                    atoi(row[duration_index].c_str()),
                    row[synopsis_index],
                    data::Cover(row[cover_normal_index], row[cover_square_index]),
                    row[video_file_index]
                );
                f(m);
            }
            catch (const std::out_of_range &e) {
                throw runtime_error("Missing column(s)");
            }
        }
    };

    csv::read(in, on_row);

    in.close();
}


/*------------------------------------------
               BASIC CATALOG
 -------------------------------------------*/

BasicCatalog::BasicCatalog() = default;

BasicCatalog::BasicCatalog(const string& filename) {
    parse_csv(filename, [&](data::Movie &m) -> void {
            add(make_unique<data::Movie>(
                m.title(),
                m.year(),
                m.category(),
                m.producer(),
                m.director(),
                m.actors(),
                m.duration(),
                m.synopsis(),
                m.cover(),
                m.video_file()
            ));
        }
    );
}

void BasicCatalog::add(unique_ptr<data::Movie> m) {
    // checks first if already exist in this catalog
    if (!get_movie(m.get()->title()))
        _data.push_back(move(m));
}

void BasicCatalog::remove(const string &title) {
    size_t i = 0;
    while (i < _data.size() && _data[i].get()->title() != title) i++;
    
    if (i != _data.size())
        _data.erase(next(_data.begin(), i));
}

size_t BasicCatalog::size() const { return _data.size(); }

optional<size_t> BasicCatalog::get_index(const string &title) const {
    for (size_t i = 0; i < _data.size(); i++) {
        if (_data[i].get()->title() == title) {
            return i;
        }
    }
    return nullopt;
}

optional<data::movie_ref> BasicCatalog::get_movie(
    const size_t index
) const {
    return ref(*(_data.at(index)));
}

bool BasicCatalog::exists(const string &title) const {
    return get_index(title).has_value();
}

optional<data::movie_ref> BasicCatalog::get_movie(
    const string &title
) const {
    optional<size_t> index = get_index(title);
    if (!index.has_value()) return nullopt;
    else return get_movie(index.value());
}

vector<data::movie_ref> BasicCatalog::movies_slice(
    size_t offset, size_t count
) const {
    vector<data::movie_ref> result;
    if (offset >= _data.size()) return result;

    size_t end = min(offset + count, _data.size());
    result.reserve(end - offset);

    for (size_t i = offset; i < end; i++)
        result.push_back(ref(*_data[i]));

    return result;
}

vector<data::movie_ref> BasicCatalog::all_movies() const {
    vector<data::movie_ref> v;
    v.reserve(_data.size());
    for (const auto &movie: _data)
        v.push_back(ref(*movie));
    return v;
}

void BasicCatalog::save(const string &filename) const {
    ofstream out(filename);
    if (!out.is_open()) throw runtime_error("Cannot open file: " + filename);

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
        csv::write_row(out, {
            movie->title(),
            to_string(movie->year()),
            movie->category(),
            movie->director(),
            movie->producer(),
            to_string(movie->duration()),
            movie->actors(),
            movie->synopsis(),
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

    parse_csv(filename, [&](data::Movie &m)->void {
        add (make_unique<data::Movie>(
            m.title(),
            m.year(),
            m.category(),
            m.producer(),
            m.director(),
            m.actors(),
            m.duration(),
            make_unique<data::CSVFileSynopsisProvider>(m.title(), filename),
            m.cover(),
            m.video_file()
        ));
    });
}

void CachedCatalog::add(unique_ptr<data::Movie> movie) {
    auto chgt_fct = [&](unique_ptr<data::SynopsisProvider> base) {
        return make_unique<CachedSynopsisProvider>(
            movie.get()->title(),
            move(base),
            [&](string t) -> string {
                auto res = get_synopsis_using_cache(t);
                return (res.has_value()) ? res.value() : "";
            });
    };

    movie.get()->change_synopsis_provider(chgt_fct);
    BasicCatalog::add(move(movie));
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
    optional<data::movie_ref> f = get_movie(title);
    if (!f.has_value()) return nullopt;

    auto &provider_ref = f->get().get_synopsis_provider().get();
    auto &csp = static_cast<CachedSynopsisProvider&>(provider_ref);
    string s = csp.get_base_provider().get().get_synopsis();
        
    // 3. evict if cache full
    if (_cache.size() >= _cache_size) {
        auto old = _order.front();
        _order.erase(_order.begin());
        _cache.erase(old);
    }

    // 4. insert into cache
    _cache[title] = s;
    _order.push_back(title);

    return s;
}

bool CachedCatalog::is_cached(const string &title) const {
    auto it = _cache.find(title);
    return it != _cache.end();
}


/*------------------------------------------
           PAGED CACHED CATALOG
 -------------------------------------------*/

// PagedCachedCatalog caches whole pages of synopsis (page = 10 movies).
PagedCachedCatalog::PagedCachedCatalog(
    const string &filename, size_t cache_size
): _filename(filename), _cache_size(cache_size) {
    parse_csv(filename, [&](data::Movie &m)->void {
        add(make_unique<data::Movie>(
            m.title(),
            m.year(),
            m.category(),
            m.producer(),
            m.director(),
            m.actors(),
            m.duration(),
            make_unique<data::CSVFileSynopsisProvider>(m.title(), filename),
            m.cover(),
            m.video_file()
        ));
    });
}

void PagedCachedCatalog::add(unique_ptr<data::Movie> movie) {
    auto chgt_fct = [&](unique_ptr<data::SynopsisProvider> base) {
        return make_unique<CachedSynopsisProvider>(
            movie.get()->title(),
            move(base),
            [&](string t) -> string {
                auto res = get_synopsis_using_cache(t);
                return (res.has_value()) ? res.value() : "";
            });
    };
    movie.get()->change_synopsis_provider(chgt_fct);
    BasicCatalog::add(move(movie));
}

bool PagedCachedCatalog::is_cached(const string &title) const {
    optional<size_t> index = get_index(title);
    if (!index.has_value()) return false;
    auto itpage = _cache.find(index.value() / PAGE_SIZE);
    return itpage != _cache.end();
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
    if (!m.has_value()) return nullopt;

    auto &prov = m.value().get().get_synopsis_provider().get();
    auto &csp = static_cast<CachedSynopsisProvider&>(prov);
    return csp.get_base_provider().get().get_synopsis();
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
        auto &m = get_movie(i).value().get();
        temp_page[m.title()] = nullopt;
    }
    
    // 3. parse CSV to fill synopses
    parse_csv(_filename, [&temp_page](data::Movie &m) -> void {
        if (temp_page.count(m.title())) {
            temp_page[m.title()] = m.synopsis();
        }
    });

    // 4. fallback: ask Movie objects directly if missing
    for (size_t i = PAGE_SIZE * index; i < last_index; i++) {
        auto &m = get_movie(i).value().get();
        if (temp_page[m.title()] == nullopt) {
            auto &csp = static_cast<CachedSynopsisProvider&>(
                m.get_synopsis_provider().get()
            );
            temp_page[m.title()] = csp.get_base_provider().get().get_synopsis();
        }
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
