/**
 * @file MongoInterface.h
 * @author Cameron Givler <cameron.givler@duke.edu>
 * @version 2.0
 *
 * Interface class to MongoDB
 *
 * A class to connect to a MongoDB database with methods to insert, retrieve,
 * and remove entries with JsonBox Values. 
 *
 * Reference: http://docs.mongodb.com/getting-started/cpp/client
 */
#pragma once

#include <cstdlib>
#include <iostream>
#include "JsonBox.h"
#include <bson.h>
//#include <mongoc.h>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/types.hpp>

/**
 * \brief Namespace for the mongoAPI code
 **/
namespace mongoAPI 
{

   #define DEFAULT_PORT 27017

/**
 * @class MongoInterface
 *
 * @brief An interface to a MongoDB database
 * with methods to insert, retrieve, and remove entries with JsonBox Values.
 */
class MongoInterface {
   private:
        JsonBox::Value m_dbInfo;           //local database info (db name, uri, port)
   
        mongocxx::database m_db;           //!< Database we are connected to
	mongocxx::client   m_conn{};       //!< Database client


	bsoncxx::document::value BSON_from_JSON(JsonBox::Value data);
	JsonBox::Value JSON_from_BSON(bsoncxx::document::value data);

   public:
	MongoInterface(std::string database="", std::string uri="", size_t port=DEFAULT_PORT );
	~MongoInterface();

	bool connect(std::string database, std::string uri, size_t port = DEFAULT_PORT );

	bool insertJSON(std::string collection, JsonBox::Value & data);
	JsonBox::Value query(std::string collection, JsonBox::Value & data);
	bool removeEntry(std::string collection
                        , JsonBox::Value & data
			, bool onlyOne
                        );
	bool update(std::string collection
                   , JsonBox::Value & query
                   , JsonBox::Value & update
                   , bool onlyOne
                   );
	JsonBox::Value getDBInfo();
   };

   bool mongoInterfaceTest();
}

