#ifndef SORT_H
#define SORT_H

#include <vector>

#include "movie.h"

/**
 * \file sort.h
 * \brief Defines sorting and selection functions for movies.
 *
 * Provides utilities to sort a list of movies by different fields
 * and to select movies according to specific criteria.
 */

namespace core {

    /**
     * \namespace sorting
     * \brief Functions to sort a vector of movies by various fields.
     */
    namespace sorting {

        /// Type alias for a movie comparison function.
        using sort_func = bool(*)(const data::movie_ref, const data::movie_ref);

        /**
         * \brief Sort a vector of movies using the provided comparison function.
         * \param movies Pointer to the vector of movies to sort.
         * \param cmp Comparison function.
         */
        void sort(std::vector<data::movie_ref> &, sort_func);

        /**
         * \brief Get a comparison function to sort movies by title.
         * \param ascending True for ascending order, false for descending.
         * \return Comparison function.
         */
        sort_func sort_by_title(bool ascending = true);

        /**
         * \brief Get a comparison function to sort movies by year.
         * \param ascending True for ascending order, false for descending.
         * \return Comparison function.
         */
        sort_func sort_by_year(bool ascending = false);

        /**
         * \brief Get a comparison function to sort movies by category/genre.
         * \param ascending True for ascending order, false for descending.
         * \return Comparison function.
         */
        sort_func sort_by_category(bool ascending = true);

        /**
         * \brief Get a comparison function to sort movies by director.
         * \param ascending True for ascending order, false for descending.
         * \return Comparison function.
         */
        sort_func sort_by_director(bool ascending = true);

        /**
         * \brief Get a comparison function to sort movies by duration.
         * \param ascending True for ascending order, false for descending.
         * \return Comparison function.
         */
        sort_func sort_by_duration(bool ascending = true);

    } // namespace sorting

    /**
     * \namespace selection
     * \brief Functions to select movies from a vector according to criteria.
     */
    namespace selection {

        /**
         * \brief Select movies with a specific title.
         * \param movies Vector of movies to search.
         * \param value Title to match.
         * \return Vector of movies with the specified title.
         */
        std::vector<data::movie_ref> select_by_title(
            const std::vector<data::movie_ref> &movies, const std::string &value);
        
        /**
         * \brief Select movies released in a specific year, optionally within a range.
         * \param movies Vector of movies to search.
         * \param value Year to match.
         * \param delta Optional range around the year (default 0 for exact match).
         *              The final interval is [value-delta; value+delta].
         * \return Vector of movies within the specified year range.
         */
        std::vector<data::movie_ref> select_by_year(
            const std::vector<data::movie_ref> &movies, int value, int delta = 0);
        
        /**
         * \brief Select movies with a specific category/genre.
         * \param movies Vector of movies to search.
         * \param value Category to match.
         * \return Vector of movies with the specified category.
         */
        std::vector<data::movie_ref> select_by_category(
            const std::vector<data::movie_ref> &movies, const std::string &value);

        /**
         * \brief Select movies directed by a specific director.
         * \param movies Vector of movies to search.
         * \param value Director name to match.
         * \return Vector of movies directed by the specified director.
         */
        std::vector<data::movie_ref> select_by_director(
            const std::vector<data::movie_ref> &movies, const std::string &value);

        /**
         * \brief Select movies with a specific duration, optionally within a delta.
         * \param movies Vector of movies to search.
         * \param value Duration in minutes to match.
         * \param delta Allowed deviation from the target duration.
         *              The final interval is [value-delta; value+delta].
         * \return Vector of movies matching the duration criteria.
         */
        std::vector<data::movie_ref> select_by_duration(
            const std::vector<data::movie_ref> &movies, int value, int delta);

    } // namespace selection

} // namespace core

#endif // SORT_H
