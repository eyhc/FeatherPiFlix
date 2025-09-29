#ifndef CSV_HPP
#define CSV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <optional>

/**
 * \file csv.h
 * \brief Defines some useful methods to handle csv stream (or file).
 */

namespace core {

    namespace csv {

        /**
         * \brief Exception to signal an early stop in CSV parsing.
         *
         * Throw this exception from a row_callback to immediately stop parsing
         * when the desired data has been found.
         */
        struct FoundException : public std::exception {};

        /**
         * \brief Callback type called for each row during CSV parsing.
         *
         * The callback receives a vector of strings representing the fields
         * of the current row. Modifying the vector inside the callback
         * does not affect the parser.
         * 
         * \throw FoundException If the callback wants to stop parsing early.
         */
        using row_callback = std::function<void(std::vector<std::string>&)>;

        /**
         * \brief Read a CSV stream chunk by chunk and call a callback for 
         *        each row.
         *
         * This function parses the input stream using libcsv.
         * Each row is converted into a vector of strings and passed 
         * to the provided callback.
         *
         * \param in The input stream containing CSV data.
         * \param on_row A function called for each parsed row.
         * Each row is a vector of strings.
         *
         * \throw std::runtime_error If libcsv encounters a parsing error.
         */
        void read(std::istream &in, row_callback on_row);

        /**
         * \brief Write a single CSV field to an output stream, escaping as 
         *        necessary.
         *
         * \param out The output stream to write to.
         * \param cell The string content of the field.
         */
        void write_field(std::ostream &out, const std::string &cell);

        /**
         * \brief Write a single row to a CSV output stream.
         *
         * Fields are written separated by commas, with proper escaping using 
         * previous \c write_field method.
         *
         * \param out The output stream to write to.
         * \param fields Vector of strings representing the row's fields.
         */
        void write_row(
            std::ostream &out,
            const std::vector<std::string> &fields
        );

        /**
         * \brief Write multiple rows to a CSV output stream.
         *
         * \param out The output stream to write to.
         * \param data A vector of rows, each row being a vector of fields.
         */
        void write(std::ostream &out, 
            const std::vector<std::vector<std::string>> &data);

        /**
         * \brief Retrieve the value of a specific field in a CSV row 
         *        identified by an ID.
         *
         * Reads the CSV from the input stream `in` line by line, searches for 
         * the row where the column `id_column_name` matches the given `id`, and
         * returns the value of the column `field_column_name`.
         *
         * \param in Input CSV stream.
         * \param id The row identifier to search for.
         * \param id_column_name Name of the column used to match the ID.
         * \param field_column_name Name of the column whose value should be 
         *        retrieved.
         *
         * \return An optional containing the value if the row and column are 
         *         found, or std::nullopt if no matching row is found.
         *
         * \throw std::runtime_error If the `id_column_name` or 
         *        `field_column_name` columns do not exist on the CSV stream
         *
         * \note This function reads the CSV sequentially and stops as soon as 
         *       the matching row is found.
         */
        std::optional<std::string> get_field(
            std::istream &in,
            const std::string &id,
            const std::string &id_column_name,
            const std::string &field_column_name
        );

        /**
         * \brief Update a specific value in a CSV stream.
         *
         * Reads the input CSV stream, searches for the row where 
         * `id_column_name` matches `id`, updates `value_column_name` to 
         * `value`, and writes the updated CSV to `out`.
         *
         * \param in Input CSV stream.
         * \param out Output stream to write the updated CSV.
         * \param id The row identifier to search for.
         * \param value The new value to write.
         * \param id_column_name Name of the column to match the id.
         * \param value_column_name Name of the column to update.
         *
         * \note edition is done only with the first match
         * 
         * \throw std::runtime_error If the `id_column_name` or 
         *        `field_column_name` columns do not exist on the CSV stream
         */
        void edit_field(
            std::istream &in,
            std::ostream &out,
            const std::string &id,
            const std::string &value,
            const std::string &id_column_name,
            const std::string &value_column_name
        );
    } // namespace csv

    /**
     * \brief Converts a string into a "slug" suitable for URLs or identifiers.
     * 
     * This function performs the following transformations on the input string:
     * 1. Replaces accented or special characters (e.g., à, é, ü, ç) with their
     *    ASCII equivalents (e.g., a, e, u, c).
     * 2. Converts all characters to lowercase.
     * 3. Replaces any non-alphanumeric characters with underscores ('_').
     * 
     * Example:
     * @code
     * std::string s = core::slug("Bonjour, le Monde!");
     * // s == "bonjour__le_monde_"
     * @endcode
     * 
     * @param src The input string to be converted into a slug.
     * @return A slugified version of the input string containing only lowercase
     *         letters, digits, and underscores.
     */
    std::string slug(const std::string &src);

} // namespace core

#endif // CSV_HPP
