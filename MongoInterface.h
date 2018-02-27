/**
 * @file mongoapi.h
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
#include "MongoAPI.h"
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
   class MongoInterface : public MongoAPI {
   private:
      std::string m_URI;                        //!< URI of the mongodb instance
      std::string m_database;                   //!< Name of the database to use
      int m_maxClients;                         //!< Maximum number of mongocxx clients
      std::atomic<uint16_t> m_createdClients;   //!< Number of created clients
      static mongocxx::instance m_instance;     //!< mongocxx driver for the instance
      mongocxx::uri m_uri;                      //!< mongocxx representation of the URI
      mongocxx::pool m_pool;              
      mongocxx::pool::entry m_client;
      mongocxx::database m_db;
      atl::TSQueue<MongoDatabaseClientPtr> m_clients;

      static bsoncxx::document::value BSON_from_JSON(JsonBox::Value data);
      static JsonBox::Value JSON_from_BSON(bsoncxx::document::view data);
   public:
      /**
       * Constructor 
       */
      MongoInterface(std::string URI = "127.0.0.1:27017", int maxClients=10);
//      ~MongoInterface();
      bool createIndex(std::string collection, JsonBox::Value index);
      bool connect(std::string database);
      std::string insert(std::string collection, JsonBox::Value data);
      int insertMany(std::string collection, std::vector<JsonBox::Value> data);
      //std::string insertUnitTests(std::string collection, JsonBox::Value data);
      JsonBox::Value query(std::string collection, JsonBox::Value data);
      JsonBox::Value queryAll(std::string collection);
      bool removeEntry(std::string collection, JsonBox::Value data);
      bool update( std::string collection
                 , JsonBox::Value filter
                 , JsonBox::Value update);
      int count(std::string collection);
      int countFilter(std::string collection, JsonBox::Value filter);
      std::string getDatabase() const;
      std::string getURI() const;
      int getMaxClients() const;
      int getCreatedClients() const;
      bool removeAllEntries(std::string collection);
      bool dropCollection(std::string collection);

   };

   std::shared_ptr<MongoAPI> getMongoDBInterface( std::string uri );
}

#endif /* MONGOINTERFACE_H_ */

