#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>
#include <xapian.h>

#include "movie.h"

/**
 * \file indexer.h
 * \brief Interface and implementation of a simple movie indexer using Xapian.
 */

namespace core::search {

    /**
     * \class Indexer
     * \brief A simple indexer for storing and searching Movie data using Xapian.
     * 
     * This class wraps a Xapian writable database and a TermGenerator to provide
     * basic indexing operations such as adding, editing, and removing movies.
     * It also allows retrieving some statistics about the database.
     */
    class Indexer {
    public:
        /**
         * \brief Constructs an Indexer and opens the database at the given path.
         * 
         * \param db_path Path to the Xapian writable database.
         * \param lang Language code (e.g., "en", "fr") used for text stemming 
         *             and analysis. Refer to the xapian's doc.
         */
        Indexer(const std::string &db_path, const std::string &lang);

        /**
         * \brief Destructor for Indexer.
         * 
         * Commits any changes and closes the Xapian database.
         */
        ~Indexer();

        /**
         * \brief Returns the total number of indexed terms in the database.
         * 
         * \return Number of terms currently indexed.
         */
        size_t nb_terms();

        /**
         * \brief Returns the total number of movies (documents) in the database.
         * 
         * \return Number of indexed movies.
         */
        size_t nb_movies();

        /**
         * \brief Adds a new movie to the database.
         * 
         * \param f Reference to the Movie object to index.
         */
        void add(const data::movie_ref &m);

        /**
         * \brief Edits an existing movie in the database.
         * 
         * The movie is identified by its old title, which is used to locate the document.
         * The existing entry is replaced with the new movie data.
         * 
         * \param old_title Title of the movie to replace.
         * \param f Reference to the new Movie object.
         */
        void edit(const std::string old_title, data::movie_ref &m);

        /**
         * \brief Removes a movie from the database by its title.
         * 
         * \param title Title of the movie to remove.
         */
        void remove(const std::string &title);

        /**
         * \brief Save modifications made to the database.
         */
        void flush();

        /**
         * \brief Erase all documents in the database
         */
        void clear();

        /**
         * \brief Search the indexed movies using a query string and return 
         *        relevance scores.
         * 
         * This method searches all indexed fields (title, year, director, etc.)
         * and returns a vector of pairs where each pair contains:
         * - the stored data (e.g., movie title or info)
         * - the relevance score of the document for the query
         * 
         * The results are sorted by descending relevance (highest score first).
         * 
         * Example usage:
         * @code
         * Indexer indexer("mydb", "en");
         * auto results = indexer.search("matrix");
         * for (const auto &r : results) {
         *     std::cout << "Title: " 
         *               << r.first 
         *               << " | Score: " 
         *               << r.second 
         *               << std::endl;
         * }
         * @endcode
         * 
         * @param query_str The query string to search for.
         * @param max_results Maximum number of results to return (default: 10).
         * @return std::vector<std::pair<std::string, double>> 
         *         A vector of pairs containing stored data and the relevance
         *         score for each matching document.
         */
        std::vector<std::pair<std::string, double>> 
            search(const std::string &query, size_t max_results = 10);

    private:
        /// Path to the Xapian database
        const std::string _db_path;

        /// Writable Xapian database
        Xapian::WritableDatabase _db;

        /// Term generator for indexing text
        Xapian::TermGenerator _termgen;

        /// Indexer lang
        const std::string _lang;
    };
        
} // namespace core::search

#endif // SEARCH_H
