/**
 * @file MongoInterface.h
 * @author Cameron Givler <cameron.givler@duke.edu>
 * @version 1.0
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
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <assert.h>
#include <aquetitools/Timer.h>
#include <mongoapi/revision.h>
#include <aquetitools/revision.h>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

namespace mongoapi
{
	/**
	 * @class MongoInterface
	 *
	 * @brief An interface to a MongoDB database
	 * with methods to insert, retrieve, and remove entries with JsonBox Values.
	 */
	class MongoInterface {
	private:
		std::string m_database;
		std::string m_URI;
		mongocxx::instance m_instance{};
		mongocxx::uri m_uri;
		mongocxx::client m_client;
		mongocxx::database m_db;

		/**
		 * Helper method to convert a JsonBox Value to a BSON object that MongoDB accepts
		 *
		 * @param data the JsonBox Value to convert
		 * @return the BSON object
		 */
		static bsoncxx::document::value BSON_from_JSON(JsonBox::Value data);
		/**
		 * Helper method to convert a BSON object from MongoDB to a JsonBox Value
		 *
		 * @param data the BSON object to convert
		 * @return the JsonBox Value
		 */
		static JsonBox::Value JSON_from_BSON(bsoncxx::document::view data);
		/**
		 * Helper method to connect to a specified database
		 *
		 * @param database The name of the database to connect to
		 * @param URI The IP address and port given as a string in the form "IP:Port"
		 * @return true if the connection was successful
		 */
		bool connect(std::string database, std::string URI);
	public:
		/**
		 * Constructor opens connection to database with specified name.
		 *
		 * @param database Name of the database to connect to
		 * @param URI The IP address and port given as a string in the form "IP:Port"
		 */
		MongoInterface(std::string database, std::string URI = "127.0.0.1:27017");
		/**
		 * Destructor
		 */
		virtual ~MongoInterface();
		/**
		 * Insert a JsonBox Value into the database with specified collection.
		 *
		 * @param collection The name of the collection to insert Value into
		 * @param data The JsonBox Value to insert
		 * @return true on success
		 */
		bool insertJSON(std::string collection, JsonBox::Value data);
		/**
		 * Query the specified collection according to a specified JsonBox Value
		 *
		 * @param collection The name of the collection to query
		 * @param data The JsonBox Value query
		 * @return Array of results
		 */
		JsonBox::Value query(std::string collection, JsonBox::Value data);
		/**
		 * Query the specified collection for all JsonBox Values
		 *
		 * @param collection The name of the collection to query
		 * @return Array of results
		 */
		JsonBox::Value queryAll(std::string collection);
		/**
		 * Remove one or multiple entries from the specified collection
		 *
		 * @param collection The name of the collection
		 * @param data A JsonBox Value specifying what entries to remove
		 * @return true on success
		 */
		bool removeEntry(std::string collection, JsonBox::Value data);
		/**
		 * this queries a collection for the specified value, and updates it with
		 * the passed parameters.  This function will only update one entry if the
		 * query matches multiple.
		 *
		 * @param collection The name of the collection
		 * @param filter Which entry to update
		 * @param update The parameters to update with
		 * @return true on success
		 */
		bool update(std::string collection, JsonBox::Value filter, 
				JsonBox::Value update);
		/**
		 * Count the number of objects in the collection specified
		 *
		 * @param collection The name of the collection to query
		 * @return integer value of number
		 */
		int count(std::string collection);
		/**
		 * Count the number of objects in the collection specified
		 * that match the specified filter
		 *
		 * @param collection The name of the collection to query
		 * @param JsonBox Value used as the filter 
		 * @return integer value of number
		 */
		int countFilter(std::string collection, JsonBox::Value filter);
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


		/****************************************************************************/
		/************************** USED FOR TESTING ONLY ***************************/
		/****************************************************************************/		

		/**
		 * Remove all entries from the specified collection
		 *
		 * @param collection The name of the collection
		 * @return true on success
		 */
		bool removeAllEntries(std::string collection);
		/**
		 * Drop specified collection
		 *
		 * @param collection The name of the collection
		 * @return true on success
		 */
		bool dropCollection(std::string collection);

	};

	std::string testMongoInterface(bool printFlag, bool assertFlag);

}

#endif /* MONGOINTERFACE_H_ */

