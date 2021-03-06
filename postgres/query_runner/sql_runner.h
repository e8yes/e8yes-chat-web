/**
 * e8yes demo web.
 *
 * <p>Copyright (C) 2020 Chifeng Wen {daviesx66@gmail.com}
 *
 * <p>This program is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * <p>You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SQL_RUNNER_H
#define SQL_RUNNER_H

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "postgres/query_runner/connection/connection_interface.h"
#include "postgres/query_runner/connection/connection_reservoir_interface.h"
#include "postgres/query_runner/orm/data_collection.h"
#include "postgres/query_runner/orm/query_completion.h"
#include "postgres/query_runner/reflection/sql_primitives.h"
#include "postgres/query_runner/resultset/result_set_interface.h"
#include "postgres/query_runner/sql_query_builder.h"

namespace e8 {
namespace sql_runner_internal {

std::string ToSearchQuery(std::string const &target_collection, std::string const &full_text_query,
                          bool prefix_search, bool rank_result, std::optional<unsigned> limit,
                          std::optional<unsigned> offset,
                          ConnectionInterface::QueryParams *query_params);

} // namespace sql_runner_internal

/**
 * @brief Query Constructs and runs query with partial information provided and synchronously
 * returns the result.
 *
 * The caller must specify the entities involved in the select query in the template argument list
 * as well as the aliases for those entities used in the select query.
 * Example usage:
 * std::vector<std::tuple<User, CreditCard>> results = Query<User, CreditCard>(query, {"auser",
 * "card"}, reservoir);
 *
 * @param query Partial query where the select list is unspecified. Example: "AUser auser JOIN
 * CreditCard card ON card.user_id = auser.id WHERE auser.join_date > '2020-1-1'"
 * @param entity_aliases A list of aliases corresponding to the entities specified in the template
 * arguments.
 * @param reservoir Connection reservoir to allocate database connections.
 * @return The query result containing a list of query record. Every record is a tuple of
 * entities in the order specified in the template.
 */
template <typename EntityType, typename... Others>
std::vector<std::tuple<EntityType, Others...>>
Query(SqlQueryBuilder const &query, std::initializer_list<std::string> const &entity_aliases,
      ConnectionReservoirInterface *reservoir) {
    std::string select_query =
        CompleteSelectQuery<EntityType, Others...>(query.PsqlQuery(), entity_aliases);

    ConnectionInterface *conn = reservoir->Take();
    std::unique_ptr<ResultSetInterface> rs = conn->RunQuery(select_query, query.QueryParams());

    std::vector<std::tuple<EntityType, Others...>> results =
        ToEntityTuples<EntityType, Others...>(rs.get());

    reservoir->Put(conn);

    return results;
}

/**
 * @brief Search Similar to the Query() function above, it constructs a full text search query with
 * partial information defining the collection of records to search from and synchronously returns
 * the result.
 *
 * @param query The query which defines the collection to search from.
 * @param entity_aliases A list of aliases corresponding to the entities specified in the template
 * arguments.
 * @param search_target_entity Target entity to be searched. It's expected that this entity has a
 * TSVECTOR column named search_terms.
 * @param full_text_query The raw full text query used to match against the record.
 * @param prefix_search Whether to perform prefix search on the full text query.
 * @param rank_result Whether to rank the search result.
 * @param limit Pagination parameter.
 * @param offset Pagination parameter.
 * @param reservoir Connection reservoir to allocate database connections.
 * @return The query result containing a list of query record. Every record is a tuple of
 * entities in the order specified in the template.
 */
template <typename EntityType, typename... Others>
std::vector<std::tuple<EntityType, Others...>>
Search(SqlQueryBuilder const &query, std::initializer_list<std::string> const &entity_aliases,
       std::string const &search_target_entity, std::string const &full_text_query,
       bool prefix_search, bool rank_result, std::optional<unsigned> limit,
       std::optional<unsigned> offset, ConnectionReservoirInterface *reservoir) {
    std::string target_collection = CompleteSelectQuery<EntityType, Others...>(
        query.PsqlQuery(), entity_aliases,
        /*augmented_select_entries=*/
        std::vector<std::string>{search_target_entity + ".search_terms"});

    ConnectionInterface::QueryParams query_params = query.QueryParams();
    std::string select_query =
        sql_runner_internal::ToSearchQuery(target_collection, full_text_query, prefix_search,
                                           rank_result, limit, offset, &query_params);

    ConnectionInterface *conn = reservoir->Take();

    // TODO: turn caching one when the search query can be parameterized.
    std::unique_ptr<ResultSetInterface> rs =
        conn->RunQuery(select_query, query_params, /*cache_on=*/false);

    std::vector<std::tuple<EntityType, Others...>> results =
        ToEntityTuples<EntityType, Others...>(rs.get());

    reservoir->Put(conn);

    return results;
}

/**
 * @brief Update Save an entity to the specified SQL table.
 *
 * @param entity Entity to be saved.
 * @param table_name Target SQL table to save to.
 * @param replace Whether or not to override existing record when conflict occurs on update.
 * @param reservoir Connection reservoir to allocate database connections.
 * @return The number of SQL rows affected by this update.
 */
uint64_t Update(SqlEntityInterface const &entity, std::string const &table_name, bool replace,
                ConnectionReservoirInterface *reservoir);

/**
 * @brief Delete Runs a deletion SQL query on the specified table.
 *
 * @param table_name  Name of the table to run deletion query on.
 * @param query  Partial deletion query. Example: "WHERE id = 100"
 * @param reservoir Connection reservoir to allocate database connections.
 * @return The number of rows deleted.
 */
uint64_t Delete(std::string const &table_name, SqlQueryBuilder const &query,
                ConnectionReservoirInterface *reservoir);

/**
 * @brief Exists Tells whethter the query returns at least one record.
 *
 * @param query The query is a partial SQL query where the SELECT ... FROM part is omitted. Example:
 * Candidates candids WHERE candids.duration > [placeholder].
 * @param reservoir Connection reservoir to allocate database connections.
 * @return True only if there is at least one record.
 */
bool Exists(SqlQueryBuilder const &query, ConnectionReservoirInterface *reservoir);

/**
 * @brief Tables Get the name of all tables in a database.
 *
 * @param reservoir Connection reservoir to allocate database connections which point to the
 * database to be reflected.
 * @return table name of the tables in the database.
 */
std::unordered_set<std::string> Tables(ConnectionReservoirInterface *reservoir);

/**
 * @brief SendHeartBeat Send a heartbeat to test the connection.
 *
 * @param reservoir Connection reservoir to allocate database connections which point to the
 * database to send heart beat to.
 */
bool SendHeartBeat(ConnectionReservoirInterface *reservoir);

/**
 * @brief SendHeartBeat Similar to the above function, but it takes in a raw connection instead of
 * the connection reservoir interface.
 */
bool SendHeartBeat(ConnectionInterface *conn);

/**
 * @brief TimeId Generate approximately unique integer ID from time. It has a resolution of
 * microsecond.
 *
 * @param host_id zero-offset ID to scatter timestamps into different ranges to avoid ID collision
 * among different host machines.
 * @return Unique ID.
 */
int64_t TimeId(unsigned host_id);

/**
 * @brief SeqId Generate sequential ID from a sequence table.
 *
 * @param seq_table Name of the sequence table to generate ID from.
 * @param reservoir Connection reservoir to allocate database connections.
 * @return Unique ID.
 */
int64_t SeqId(std::string const &seq_table, ConnectionReservoirInterface *reservoir);

/**
 * @brief ClearAllTables Delete data in all table but keeping the schema structure.
 *
 * @param reservoir Connections pointing to the database where its tables needed to be clear.
 */
void ClearAllTables(ConnectionReservoirInterface *reservoir);

} // namespace e8

#endif // SQL_RUNNER_H
