#include "core/search.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace core;

int main(void) {
    // source: allocine
    auto m1 = data::Movie(
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

    auto m2 = data::Movie(
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

    auto m3 = data::Movie(
        "La Trilogie Marseillaise : César", 1936, "Comédie dramatique",
        "Marcel Pagnol", "Marcel Pagnol", "Raimu, Pierre Fresnay, Fernand "
        "Charpin", 141, "Fanny, abandonnée par Marius, épouse Panisse qui "
        "adopte Césariot, l’enfant de l’amour, et l’élève comme son fils. "
        "Aujourd’hui, Césariot est adulte et Panisse se meurt. Fanny révèle "
        "la vérité à son fils qui décide alors de partir à la recherche de "
        "Marius, son père…", data::Cover(), ""
    );

    auto m4 = data::Movie(
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
    auto m5 = data::Movie(
        "J'accuse", 1938, "Drame", "Abel Gance, Steve Passeur", "Abel Gance", 
        "Victor Francen, Line Noro, Marie Lou", 141, "Jean Diaz, scientifique, "
        "s'est jure d'empecher une nouvelle guerre. Il invente un verre "
        "indestructible dont l'invention lui est volée et sombre dans la folie."
        " Vingt ans plus tard, il retrouve sa raison pour apprendre qu'une "
        "nouvelle guerre se prepare.", data::Cover(), ""
    );
    auto m6 = data::Movie(
        "Germinal", 1993, "Drame", "Arlette Langmann, Claude Berri",
        "Claude Berri", "Renaud, Judith Henry,  Miou-Miou", 160,
        "A la fin du XIXe siècle, Etienne Lantier arrive à Montsou, un village "
        "minier. Avec l'aide de Maheu, un ouvrier haveur, il parvient à se "
        "faire embaucher. Quand l'un des sept enfants Maheu, Zacharie, se "
        "marie, Etienne prend sa place dans la maison familiale. Il s'est lié "
        "d'amitié avec Maheu...", data::Cover(), ""
    );

    search::Indexer *index = new search::Indexer("./index_db", "french");
    auto rm1 = ref(m1);
    auto rm2 = ref(m2);
    auto rm3 = ref(m3);
    auto rm4 = ref(m4);
    auto rm5 = ref(m5);
    auto rm6 = ref(m6);

    index->add(rm1);
    index->add(rm2);
    index->add(rm3);
    index->add(rm4);
    index->add(rm5);
    index->add(rm6);

    assert(index->nb_movies() == 6);
    size_t nb_terms = index->nb_terms();
    assert(nb_terms > 20);

    string ancien = m6.title();
    m6.set_category("Fantastique");
    index->edit(ancien, rm6);
    assert(index->nb_movies() == 6);
    assert(index->nb_terms() >= nb_terms + 1);
    
    index->remove(m5.title());
    assert(index->nb_movies() == 5);
    nb_terms = index->nb_terms();
    delete index;

    index = new search::Indexer("./index_db", "french");
    assert(index->nb_movies() == 5);
    assert(index->nb_terms() == nb_terms);

    index->add(ref(m5));
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
