#include "core/media_manager.h"
#include "../utils.h"

#include <iostream>
#include <filesystem>
#include <cassert>

using namespace std;
using namespace core;

int main(void) {
    auto movies = test::create_collection();

    MediaManager *mm = new MediaManager(
        "db", "movies.csv", "french", MediaManager::cache_type::NO_CACHE);
    
    for (auto &m : test::create_collection())
        mm->add(move(m));    

    delete mm;

    mm = new MediaManager(
        "./db", "movies.csv", "fr", MediaManager::cache_type::NO_CACHE);


    // --- catalog ---

    assert(mm->nb_movies() == 6);

    assert(mm->exists("Germinal"));
    assert(!mm->exists("germinal"));

    assert(mm->get_movie("Germinal").has_value());
    assert(mm->get_movie("Germinal").value().get().year() == 1993);

    assert(mm->movies().size() == 6);
    assert(mm->movies()[0].get().title() == "La Trilogie Marseillaise : Marius");

    assert(mm->movies(2,2).size() == 2);
    assert(mm->movies(2,2)[0].get().title() == "La Trilogie Marseillaise : César");
    assert(mm->movies(2,2)[1].get().title() == "La Fin du jour");


    // --- indexer ---

    assert(mm->search("raimu").size() == 3);
    mm->remove("La Trilogie Marseillaise : Marius");
    assert(mm->nb_movies() == 5);
    assert(mm->search("raimu").size() == 2);

    mm->get_movie("La Trilogie Marseillaise : César").value().get().set_actors("");
    assert(mm->search("raimu").size() == 2);
    mm->reindex("La Trilogie Marseillaise : César");
    assert(mm->search("raimu").size() == 1);
    mm->get_movie("La Trilogie Marseillaise : César").value().get().set_actors(
        "Raimu, Charpin, Demazis, Frenet"
    );
    mm->get_movie("Germinal").value().get().set_actors("Raimu");
    mm->reindex_all();
    assert(mm->search("raimu").size() == 3);

    filesystem::remove_all("./db");
    filesystem::remove("movies.csv");
    
    cout << "TEST MEDIA MANAGER: OK" << endl;

    return 0;
}
