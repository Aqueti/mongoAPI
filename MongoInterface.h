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
#include "JsonBox.h"
//#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

/**
 * @class MongoInterface
 *
 * @brief An interface to a MongoDB database
 * with methods to insert, retrieve, and remove entries with JsonBox Values.
 */
class MongoInterface {
private:
	std::string database;
	std::string IP_Port;
	mongo::DBClientConnection connection;
	/**
	 * Helper method to convert a JsonBox Value to a BSON object that MongoDB accepts
	 *
	 * @param data the JsonBox Value to convert
	 * @return the BSON object
	 */
	static mongo::BSONObj BSON_from_JSON(JsonBox::Value data);
	/**
	 * Helper method to convert a BSON object from MongoDB to a JsonBox Value
	 *
	 * @param data the BSON object to convert
	 * @return the JsonBox Value
	 */
	static JsonBox::Value JSON_from_BSON(mongo::BSONObj data);
	/**
	 * Helper method to connect to a specified database
	 *
	 * @param database The name of the database to connect to
	 * @param IP_Port The IP address and port given as a string in the form "IP:Port"
	 * @return true if the connection was successful
	 */
	bool connect(std::string database, std::string IP_Port);
public:
	/**
	 * Constructor opens connection to database with specified name.
	 *
	 * @param database Name of the database to connect to
	 * @param IP_Port The IP address and port given as a string in the form "IP:Port"
	 */
	MongoInterface(std::string database = "aqueti", std::string IP_Port =
			"localhost");
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
	 * Remove one or multiple entries from the specified collection
	 *
	 * @param collection The name of the collection
	 * @param data A JsonBox Value specifying what entries to remove
	 * @param onlyOne If true, a maximum of one entry will be removed
	 * @return true on success
	 */
	bool removeEntry(std::string collection, JsonBox::Value data,
			bool onlyOne);
	/**
	 * this queries a collection for the specified value, and updates it with
	 * the passed parameters.  This function will only update one entry if the
	 * query matches multiple.
	 *
	 * @param collection The name of the collection
	 * @param query Which entry to update
	 * @param update The parameters to update with
	 * @return true on success
	 */
	bool update(std::string collection, JsonBox::Value query,
			JsonBox::Value update, bool onlyOne);
	/**
	 * Returns a string containing the name of the current database
	 *
	 * @return The name of the current database
	 */
	std::string getDatabase() const;
	/**
	 * Change the database to query
	 *
	 * @param database The name of the new database
	 */
	void setDatabase(std::string database);
	/**
	 * Returns a string containing the IP and Port of the database
	 *
	 * @return The IP and port in the form "IP:Port"
	 */
	std::string getIP_Port() const;
};

bool mongoInterfaceTest();

#endif /* MONGOINTERFACE_H_ */
