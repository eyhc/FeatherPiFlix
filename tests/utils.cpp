#include "utils.h"
#include <curl/curl.h>

std::vector<std::byte> test::get_data(std::string url) {
    curl_global_init(CURL_GLOBAL_ALL);

    CURLcode res_code = CURLE_FAILED_INIT;
    CURL *curl = curl_easy_init();
    std::vector<std::byte> result;

    if (!curl) return result;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
        static_cast<curl_write_callback>([](
            char *contents, size_t size, size_t nmemb, void *userdata
        ) -> size_t {
            auto *data = static_cast<std::vector<std::byte>*>(userdata);
            for (size_t i = 0; i < size * nmemb; i++)
                data->push_back((std::byte)contents[i]);
            
            return size * nmemb;
        }));
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    res_code = curl_easy_perform(curl);
    if (res_code != CURLE_OK) {
        curl_global_cleanup();
        result.clear();
        return result;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return result;
}

std::vector<std::unique_ptr<core::data::Movie>> test::create_collection() {
    // source: allocine
    auto m1 = std::make_unique<core::data::Movie>(
        "La Trilogie Marseillaise : Marius", 1931, "Comédie dramatique",
        "Marcel Pagnol", "Alexander Korda, Marcel Pagnol", "Raimu, Pierre "
        "Fresnay, Fernand Charpin", 127, "Marius est un jeune homme que la mer "
        "exalte. Il aime son père César, bourru et bonhomme, il aime aussi la "
        "petite Fanny qui vend des coquillages devant le bar de César. Depuis "
        "son enfance, l’envie de courir le monde l’enflamme. Il lutte contre sa"
        " folie. Il ne veut pas abandonner son père qui en mourrait peut-être "
        "de chagrin, ni la petite Fanny qui ne pense qu’à lui. Et pourtant la "
        "mer est là…", core::data::Cover(), ""
    );
    auto m2 = std::make_unique<core::data::Movie>(
        "La Trilogie Marseillaise : Fanny", 1932,"Comédie dramatique",
         "Marcel Pagnol", "Marcel Pagnol, Marc Allégret", "Raimu, Pierre "
         "Fresnay, Orane Demazis", 126, "Marius a embarqué sur « La Malaisie »,"
         " cédant à l’irrépressible appel du large. Il a laissé derrière lui "
         "son vieux père César et sa fiancée Fanny, qui porte son enfant. Sans "
         "nouvelles de l’homme qu’elle aime depuis plusieurs mois, la jeune "
         "femme se résout à épouser le brave Honoré Panisse pour donner un père"
         " à son enfant. Très épris de Fanny et ravi de s’assurer une "
         "descendance, Panisse adopte le petit Césariot. Mais un jour, "
         "Marius revient…", core::data::Cover(), ""
    );
    auto m3 = std::make_unique<core::data::Movie>(
        "La Trilogie Marseillaise : César", 1936, "Comédie dramatique",
        "Marcel Pagnol", "Marcel Pagnol", "Raimu, Pierre Fresnay, Fernand "
        "Charpin", 141, "Fanny, abandonnée par Marius, épouse Panisse qui "
        "adopte Césariot, l’enfant de l’amour, et l’élève comme son fils. "
        "Aujourd’hui, Césariot est adulte et Panisse se meurt. Fanny révèle "
        "la vérité à son fils qui décide alors de partir à la recherche de "
        "Marius, son père…", core::data::Cover(), ""
    );
    auto m4 = std::make_unique<core::data::Movie>(
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
        "magnifique réflexion sur la vieillesse.", core::data::Cover(), ""
    );
    auto m5 = std::make_unique<core::data::Movie>(
        "J'accuse", 1938, "Drame", "Abel Gance, Steve Passeur", "Abel Gance", 
        "Victor Francen, Line Noro, Marie Lou", 141, "Jean Diaz, scientifique, "
        "s'est jure d'empecher une nouvelle guerre. Il invente un verre "
        "indestructible dont l'invention lui est volée et sombre dans la folie."
        " Vingt ans plus tard, il retrouve sa raison pour apprendre qu'une "
        "nouvelle guerre se prepare.", core::data::Cover(), ""
    );
    auto m6 = std::make_unique<core::data::Movie>(
        "Germinal", 1993, "Drame", "Arlette Langmann, Claude Berri",
        "Claude Berri", "Renaud, Judith Henry,  Miou-Miou", 160,
        "A la fin du XIXe siècle, Etienne Lantier arrive à Montsou, un village "
        "minier. Avec l'aide de Maheu, un ouvrier haveur, il parvient à se "
        "faire embaucher. Quand l'un des sept enfants Maheu, Zacharie, se "
        "marie, Etienne prend sa place dans la maison familiale. Il s'est lié "
        "d'amitié avec Maheu...", core::data::Cover(), ""
    );

    std::vector<std::unique_ptr<core::data::Movie>> vres;
    vres.emplace_back(move(m1));
    vres.emplace_back(move(m2));
    vres.emplace_back(move(m3));
    vres.emplace_back(move(m4));
    vres.emplace_back(move(m5));
    vres.emplace_back(move(m6));
    
    return vres;
}
