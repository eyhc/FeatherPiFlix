#include "core/search.h"
#include "core/utils.h"

#include "xapian.h"

using namespace std;
using namespace core;

search::Indexer::Indexer(const string &db_path, const string &lang): 
    _db_path(db_path), _db(db_path, Xapian::DB_CREATE_OR_OPEN), _lang(lang) {
    _termgen.set_stemmer(Xapian::Stem(lang));
}

search::Indexer::~Indexer() {
    _db.close();
}

size_t search::Indexer::nb_terms() {
    return distance(_db.allterms_begin(), _db.allterms_end());
}

size_t search::Indexer::nb_movies() {
    return _db.get_doccount();
}

void search::Indexer::add(const data::Movie *f) {
    Xapian::Document doc;
    doc.set_data(f->title());
    _termgen.set_document(doc);

    doc.set_data(f->title());

    _termgen.index_text(f->title(),           5); _termgen.increase_termpos();
    _termgen.index_text(f->category(),        3); _termgen.increase_termpos();
    _termgen.index_text(to_string(f->year()), 1); _termgen.increase_termpos();
    _termgen.index_text(f->director(),        2); _termgen.increase_termpos();
    _termgen.index_text(f->producer(),        2); _termgen.increase_termpos();
    _termgen.index_text(f->actors(),          4); _termgen.increase_termpos();
    _termgen.index_text(f->synopsis(),        1);

    std::string unique_id = "Q" + slug(f->title());
    doc.add_boolean_term(unique_id);

    _db.replace_document(unique_id, doc);
}

void search::Indexer::edit(const string old_title, data::Movie *f) {
    remove(old_title); add(f);
}

void search::Indexer::remove(const string &title) {
    string unique_id = "Q" + slug(title);
    _db.delete_document(unique_id);
}

vector<pair<string, double>> search::Indexer::search(
    const string &query_str, size_t max_results
) {
    vector<pair<string, double>> results;

    // Query parser
    Xapian::QueryParser qp;
    qp.set_database(_db);
    qp.set_stemmer(Xapian::Stem(_lang));
    qp.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);

    // Parse the query
    Xapian::Query query = qp.parse_query(query_str);

    // Perform the search
    Xapian::Enquire enquire(_db);
    enquire.set_query(query);

    Xapian::MSet matches = enquire.get_mset(0, max_results);

    // Collect results with relevance score
    for (auto it = matches.begin(); it != matches.end(); it++) {
        double score = it.get_weight();             // relevance score
        string data = it.get_document().get_data(); // movie title
        results.emplace_back(data, score);
    }

    return results;
}
