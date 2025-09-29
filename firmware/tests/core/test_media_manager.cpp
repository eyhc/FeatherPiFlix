#include "core/media_manager.h"

#include <iostream>
#include <filesystem>
#include <cassert>

using namespace std;
using namespace core;

int main(void) {
    // source: allocine
    auto m1 = make_unique<data::Movie>(
        "La Trilogie Marseillaise : Marius", 1931, "Comédie dramatique",
        "Marcel Pagnol", "Alexander Korda, Marcel Pagnol", "Raimu, Pierre "
        "Fresnay, Fernand Charpin", 127, "Marius est un jeune homme que la mer "
        "exalte. Il aime son père César, bourru et bonhomme, il aime aussi la "
        "petite Fanny qui vend des coquillages devant le bar de César. Depuis "
        "son enfance, l’envie de courir le monde l’enflamme. Il lutte contre sa"
        " folie. Il ne veut pas abandonner son père qui en mourrait peut-être "
        "de chagrin, ni la petite Fanny qui ne pense qu’à lui. Et pourtant la "
        "mer est là…", data::Cover(), ""
    );

    auto m2 = make_unique<data::Movie>(
        "La Trilogie Marseillaise : Fanny", 1932,"Comédie dramatique",
         "Marcel Pagnol", "Marcel Pagnol, Marc Allégret", "Raimu, Pierre "
         "Fresnay, Orane Demazis", 126, "Marius a embarqué sur « La Malaisie »,"
         " cédant à l’irrépressible appel du large. Il a laissé derrière lui "
         "son vieux père César et sa fiancée Fanny, qui porte son enfant. Sans "
         "nouvelles de l’homme qu’elle aime depuis plusieurs mois, la jeune "
         "femme se résout à épouser le brave Honoré Panisse pour donner un père"
         " à son enfant. Très épris de Fanny et ravi de s’assurer une "
         "descendance, Panisse adopte le petit Césariot. Mais un jour, "
         "Marius revient…", data::Cover(), ""
    );

    auto m3 = make_unique<data::Movie>(
        "La Trilogie Marseillaise : César", 1936, "Comédie dramatique",
        "Marcel Pagnol", "Marcel Pagnol", "Raimu, Pierre Fresnay, Fernand "
        "Charpin", 141, "Fanny, abandonnée par Marius, épouse Panisse qui "
        "adopte Césariot, l’enfant de l’amour, et l’élève comme son fils. "
        "Aujourd’hui, Césariot est adulte et Panisse se meurt. Fanny révèle "
        "la vérité à son fils qui décide alors de partir à la recherche de "
        "Marius, son père…", data::Cover(), ""
    );

    auto m4 = make_unique<data::Movie>(
        "La Fin du jour", 1939, "Drame", "Charles Spaak, Julien Duvivier",  
        "Julien Duvivier", "Louis Jouvet, Michel Simon, Victor Francen", 108,
        "Près de Tarascon, l'abbaye de Saint-Jean-la-Rivière abrite une "
        "institution charitable pour les vieux comédiens désargentés. Alors qu'"
        "une menace de fermeture plane sur la maison de retraite, Raphaël Saint-"
        "Clair, ancienne gloire du théâtre français, séducteur cynique et "
        "mythomane, vient s'y installer à son tour. Sa présence ranime bientôt "
        "certaines rancoeurs parmi les pensionnaires. Parmi eux, Marny, acteur "
        "talentueux qui n'a jamais connu le succès, dont l'épouse fut la maîtres"
        "se de Saint-Clair, et Cabrissade, cabotin farceur qui a passé sa "
        "carrière en coulisses, condamné au rôle de doublure. Une distribution "
        "exceptionnelle dans un vibrant hommage à l'art du jeu, doublé d'une "
        "magnifique réflexion sur la vieillesse.", data::Cover(), ""
    );
    auto m5 = make_unique<data::Movie>(
        "J'accuse", 1938, "Drame", "Abel Gance, Steve Passeur", "Abel Gance", 
        "Victor Francen, Line Noro, Marie Lou", 141, "Jean Diaz, scientifique, "
        "s'est jure d'empecher une nouvelle guerre. Il invente un verre "
        "indestructible dont l'invention lui est volée et sombre dans la folie."
        " Vingt ans plus tard, il retrouve sa raison pour apprendre qu'une "
        "nouvelle guerre se prepare.", data::Cover(), ""
    );
    auto m6 = make_unique<data::Movie>(
        "Germinal", 1993, "Drame", "Arlette Langmann, Claude Berri",
        "Claude Berri", "Renaud, Judith Henry,  Miou-Miou", 160,
        "A la fin du XIXe siècle, Etienne Lantier arrive à Montsou, un village "
        "minier. Avec l'aide de Maheu, un ouvrier haveur, il parvient à se "
        "faire embaucher. Quand l'un des sept enfants Maheu, Zacharie, se "
        "marie, Etienne prend sa place dans la maison familiale. Il s'est lié "
        "d'amitié avec Maheu...", data::Cover(), ""
    );

    MediaManager *mm = new MediaManager(
        "db", "movies.csv", "french", MediaManager::cache_type::NO_CACHE);
    
    mm->add(move(m1));
    mm->add(move(m2));
    mm->add(move(m3));
    mm->add(move(m4));
    mm->add(move(m5));
    mm->add(move(m6));

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
