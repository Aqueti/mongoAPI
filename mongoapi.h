/**
 * @file mongoapi.h
 * @author Nils Persson <npersson@live.unc.edu>
 * @version 2.0
 *
 * Interface class to MongoDB
 *
 * A class to connect to a MongoDB database with methods to insert, retrieve,
 * and remove entries with JsonBox Values.
 */

#ifndef MONGOINTERFACE_H_
#define MONGOINTERFACE_H_

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include "JsonBox.h"
#include <vector>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/oid.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <assert.h>
#include "atl/Timer.h"
#include "atl/TSQueue.tcc"
#include "MongoDatabaseClient.h"

// #include <revision.h>
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

namespace mongoapi
{
    typedef std::shared_ptr<MongoDatabaseClient> MongoDatabaseClientPtr;

	/**
	 * @class MongoInterface
	 *
	 * @brief An interface to a MongoDB database
	 * with methods to insert, retrieve, and remove entries with JsonBox Values.
	 */
	class MongoInterface {
	protected:
		std::string m_database;
		std::string m_URI;
		static mongocxx::instance m_instance;
		mongocxx::uri m_uri;
		mongocxx::pool m_pool;
		mongocxx::pool::entry m_client;
		mongocxx::database m_db;
        	int m_maxClients;
		std::atomic<uint16_t> m_createdClients;

	public:
        	atl::TSQueue<MongoDatabaseClientPtr> m_clients;
		/**
		 * Helper method to convert a JsonBox Value to a BSON object that MongoDB accepts
		 *
		 * @param data the JsonBox Value to convert
		 * @return The BSON object
		 */
		static bsoncxx::document::value BSON_from_JSON(JsonBox::Value data);
		/**
		 * Helper method to convert a BSON object from MongoDB to a JsonBox Value
		 *
		 * @param data the BSON object to convert in view form
		 * @return The JsonBox Value
		 */
		static JsonBox::Value JSON_from_BSON(bsoncxx::document::view data);
		/**
		 * Constructor 
		 */
		MongoInterface(std::string URI = "127.0.0.1:27017", int maxClients=10);
		/**
		 * Destructor
		 */
		virtual ~MongoInterface();
		
		/**
		 * Creates an index on a collection
		 */
		bool createIndex(std::string collection, JsonBox::Value index);
		/**
		 * Connect to a specified database
		 *
		 * @param database The name of the database to connect to
		 * @return True if the connection was successful
		 */
		bool connect(std::string database);
		/**
		 * Insert a JsonBox Value into the database with specified collection.
		 *
		 * @param collection The name of the collection to insert Value into
		 * @param data The JsonBox Value to insert
		 * @return Id of inserted value or 0 if error encountered
		 */
		std::string insert(std::string collection, JsonBox::Value data);
		/**
		 * Insert a vector of JsonBox Values into the database with specified collection.
		 *
		 * @param collection The name of the collection to insert Value into
		 * @param data The JsonBox Values to insert
		 * @return number of documents inserted, or 0 on failure
		 */
		int insertMany(std::string collection, std::vector<JsonBox::Value> data);
	
		/**
		 * Query the specified collection according to a specified JsonBox Value
		 *
		 * @param collection The name of the collection to query
		 * @param data The JsonBox Value query
		 * @return Array of results
		 * 
		 * 
		 * Here are some useful conditional examples:
		 * 
		 * JsonBox::Object myObject = {"myKey":{CONDITIONS}} 
		 * 
		 * Greater than equal CONDITONS = "$gte": 12345 returns a jsonbox array of "myKey": value such that value 
		 * is greater than 12345
		 * -----Less than equal is the same just replace the g with an l
		 * 
		 * Bound by some range CONDITIONS = "$gte": 12345, "$lte": 12348 returns a jsonbox array of "myKey": value such
		 * that value is inclusively bounded by 12345-12348
		 */
		JsonBox::Value query(std::string collection, JsonBox::Value data);

		/**
		 * Remove one or multiple entries from the specified collection
		 *
		 * @param collection The name of the collection
		 * @param data A JsonBox Value specifying what entries to remove
		 * @return True on success
		 */
		bool removeEntries(std::string collection, JsonBox::Value data);
		/**
		 * this queries a collection for the specified value, and updates it with
		 * the passed parameters.  This function will only update one entry if the
		 * query matches multiple.
		 *
		 * @param collection The name of the collection
		 * @param filter Which entry to update
		 * @param update The parameters to update with
		 * @return True on success
		 */
		bool update(std::string collection, JsonBox::Value filter, 
				JsonBox::Value update);
		/**
		 * Count the number of objects in the collection specified
		 * that match the specified filter
		 *
		 * @param collection The name of the collection to query
		 * @param filter The query used on collection  
		 * @return Integer value of number
		 */
		int count(std::string collection, JsonBox::Value filter);
		/**
		 * Returns a string containing the name of the current database
		 *
		 * @return The name of the current database
		 */
		std::string getDatabase() const;
		/**
		 * Returns a string containing the IP and Port of the database
		 *
		 * @return The IP and port in the form "IP:Port"
		 */
		std::string getURI() const;

		int getMaxClients() const;

		int getCreatedClients() const;

		/**
		 * Drop specified collection
		 *
		 * @param collection The name of the collection
		 * @return True on success
		 */
		bool dropCollection(std::string collection);

	};
}

#endif /* MONGOINTERFACE_H_ */

