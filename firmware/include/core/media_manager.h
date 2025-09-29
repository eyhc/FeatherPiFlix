#ifndef MEDIA_MANAGER_H
#define MEDIA_MANAGER_H

#include "search.h"
#include "catalog.h"

/**
 * \file media_manager.h
 * \brief Defines MediaManager class to unify catalog and indexer management.
 */

namespace core {

    /**
     * \class MediaManager
     * \brief High-level manager for movie catalogs and search indexing.
     * 
     * This class provides unified access to a collection of movies stored
     * in a catalog, with optional synopsis caching strategies, and integrates
     * full-text search capabilities.
     */
    class MediaManager {
    public:
        /**
         * \enum cache_type
         * \brief Cache strategies available for managing movie synopses.
         */
        enum cache_type { 
            NO_CACHE,         ///< No caching.
            INDIVIDUAL_CACHE, ///< Individual LRU cache (per-synopsis).
            PAGED_CACHE       ///< Paged LRU cache (pages of 10 movies).
        };

        /**
         * \brief Construct a MediaManager.
         * \param index_database Path to the indexer database.
         * \param movies_csv_file Path to the list of movies in CSV file format.
         * \param lang Language code used for indexing and search.
         * \param catalog_type Caching strategy for the catalog 
         *                     (see \c cache_type).
         * \param cache_size Cache capacity (number of entries or pages, 
         *                   depending on `catalog_type`).
         */
        MediaManager(
            const std::filesystem::path &index_database,
            const std::filesystem::path &movies_csv_file,
            const std::string &lang,
            cache_type catalog_type,
            size_t cache_size = 10
        );

        /**
         * \brief Destructor.
         * Releases owned resources (catalog and index).
         */
        ~MediaManager();


        // --- ACCESSORS ---

        /**
         * \brief Check whether a movie exists in the catalog.
         * \param title Title of the movie.
         * \return True if the movie exists, false otherwise.
         */
        bool exists(const std::string &title) const;

        /**
         * \brief Get a reference to a movie by title.
         * \param title Title of the movie.
         * \return A reference to the movie if found, or empty optional otherwise.
         */
        std::optional<data::movie_ref> get_movie(const std::string &title) const;

        /**
         * \brief Get references to all movies in the catalog.
         * \return A vector of references to all movies.
         */
        std::vector<data::movie_ref> movies() const;

        /**
         * \brief Get a subset of movies by chunks.
         * \param offset Starting index (0-based).
         * \param count Maximum number of movies to return.
         * \return A vector of references to the selected movies.
         */
        std::vector<data::movie_ref> movies(size_t offset, size_t count) const;

        /**
         * \brief Get the number of movies in the catalog.
         * \return Total number of movies.
         */
        size_t nb_movies() const;


        // --- DATA MANAGEMENT ---

        /**
         * \brief Add a movie to the catalog and index.
         * \param m Movie to add (ownership transferred).
         */
        void add(std::unique_ptr<data::Movie> m);

        /**
         * \brief Remove a movie from the catalog and index.
         * \param title Title of the movie to remove.
         */
        void remove(const std::string &title);

        /**
         * \brief Reindex a single movie in the search index.
         * \param title Title of the movie to reindex.
         * \note Please, call it whenever you edit a movie.
         */
        void reindex(const std::string &title);

        /**
         * \brief Reindex all movies in the catalog.
         */
        void reindex_all();

        /**
         * \brief Flush pending data and ensure consistency.
         * Forces the catalog and index to be saved or synchronized with disk.
         */
        void flush();

        /**
         * \brief Perform a ranked search query in the index.
         * \param query Search string.
         * \param max_result Maximum number of results to return.
         * \return A vector of pairs (movie reference, relevance), 
         *         sorted by descending relevance.
         */
        std::vector<std::pair<data::movie_ref, double>> search(
            std::string query, size_t max_result = 10) const;

    private:
        BasicCatalog *_movies;    ///< Catalog of movies (may be cached).
        search::Indexer *_index;  ///< Full-text search index.

        /// CSV file associated with the catalog.
        std::filesystem::path _csv_file;
    };
} // namespace core

#endif // MEDIA_MANAGER_H
