/**
 * @file mongoapi.h
 * @version 1.0
 *
 * Interface class to MongoDB
 *
 * A class to connect to a MongoDB database with methods to insert, retrieve,
 * and remove entries with JsonBox Values.
 */

#include "MongoInterface.h"

namespace mongoapi
{



mongocxx::instance MongoInterface::m_instance{};

/**
 * \brief Constructure
 * \param [in] URI address and port of the mongodb server. Default: 127.0.0.1:27017
 * \param [in] maxClients maximum number of database clients. Used for performance optimization
 * \return none
 **/
MongoInterface::MongoInterface(std::string URI, int maxClients) : 
    m_uri("mongodb://" + URI + "/?minPoolSize=1&maxPoolSize=" + std::to_string(maxClients+1))
    , m_pool(m_uri)
{
   this->m_URI = URI;
   m_maxClients = maxClients;
   m_createdClients = 0;
}

/**
 * \brief standard destructor
 *
MongoInterface::~MongoInterface()
{
}
*/
/**
 * \brief specify which database to use use on the server
 * \param [in] database name of the database
 * \return true on success, false on failure
 **/
bool MongoInterface::connect(std::string database) 
{
   m_database = database;
   try{
      // (re)acquire a local instance to use for high-level maintenance functions
      m_client = m_pool.acquire();
      m_db = (*m_client)[database];

      // clear the current queue of existing instances
      m_clients.delete_all();
      m_createdClients = 0;

      // acquire a queue of connected instances for query/insert/etc.
      for( int i = 0; i < m_maxClients; i++ ){
         MongoDatabaseClientPtr dbc = std::make_shared<MongoDatabaseClient>(m_pool, database);
         m_clients.enqueue(dbc);
         m_createdClients++;
      }
      
   }
   catch(const mongocxx::exception& e){
      std::cout << "database connection failed after creating " 
           << m_createdClients << " database clients" << std::endl;
      return false;
   }
   return true;
}

/**
 * Helper method to convert a JsonBox Value to a BSON object that MongoDB accepts
 *
 * @param data the JsonBox Value to convert
 * @return The BSON object
 **/
bsoncxx::document::value MongoInterface::BSON_from_JSON(JsonBox::Value data)
{
   std::stringstream stream;
   data.writeToStream(stream, false);
   return bsoncxx::from_json(stream.str());
}

/**
 * Helper method to convert a BSON object from MongoDB to a JsonBox Value
 *
 * @param data the BSON object to convert in view form
 * @return The JsonBox Value
 */
JsonBox::Value MongoInterface::JSON_from_BSON(bsoncxx::document::view data)
{
   JsonBox::Value j;
   j.loadFromString(bsoncxx::to_json(data));
   return j;
}

/**
 * \brief creates an index in a database collection. This is used to optimize lookups
 * \param [in] collection name of the collection to index
 * \param [in] index JsonBox string that specifies how to index. (Need more details)
 * \return true on success, false on failure
 **/
bool MongoInterface::createIndex(std::string collection, JsonBox::Value index)
{
   try{
      m_db[collection].create_index(BSON_from_JSON(index));
      return true;
   } catch( const mongocxx::logic_error& e ){
      std::cout << "createIndex: " << e.what() << std::endl;
   } catch( const mongocxx::operation_exception& e ){
      std::cout << "createIndex: " << e.what() << std::endl;
   }
   return false;
}

/**
 * Insert a JsonBox Value into the database with specified collection.
 *
 * @param collection The name of the collection to insert Value into
 * @param data The JsonBox Value to insert
 * @return Id of inserted value or 0 if error encountered
 **/
std::string MongoInterface::insert(std::string collection,
      JsonBox::Value data)
{
    std::string strReturn;
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return strReturn;
    }

   try {
      mongocxx::collection coll = dbc->m_db[collection];
      mongocxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(BSON_from_JSON(data));
      if(result){
         bsoncxx::types::value id = result->inserted_id();
         if(id.type() == bsoncxx::type::k_oid){
            bsoncxx::oid oid = id.get_oid().value;
            strReturn = oid.to_string();
         }
      }
   } catch (const mongocxx::bulk_write_exception& e) {
      std::cout << "sdf - insert: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "insert: default exception" << std::endl;
   }

    m_clients.enqueue(dbc);
   return strReturn;
}

/**
 * Insert a vector of JsonBox Values into the database with specified collection.
 *
 * @param collection The name of the collection to insert Value into
 * @param data The JsonBox Values to insert
 * @return number of documents inserted, or 0 on failure
 **/
int MongoInterface::insertMany(std::string collection,
      std::vector<JsonBox::Value> data)
{
    int ret = 0;
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return ret;
    }

   try {
      // convert vector to BSON
      std::vector<bsoncxx::document::value> documents;
      for( JsonBox::Value doc : data ){
          documents.push_back(BSON_from_JSON(doc));
      }

      mongocxx::collection coll = dbc->m_db[collection];
      mongocxx::stdx::optional<mongocxx::result::insert_many> result = coll.insert_many(documents);
      if(result){
         ret = result->inserted_count();
         //bsoncxx::types::value id = result->inserted_id();
         //if(id.type() == bsoncxx::type::k_oid){
         //   bsoncxx::oid oid = id.get_oid().value;
         //   strReturn = oid.to_string();
         //}
         //std::cout << "INSERTED " << ret << " documents" << std::endl;
      }
   } catch (const mongocxx::bulk_write_exception& e) {
      std::cout << "insert: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "insert: default exception" << std::endl;
   }

    m_clients.enqueue(dbc);
   return ret;
}

/*
 * Insert submodules into specified collection as individual documents for unit testing.
 *
 * @param collection The name of the collection to insert Value into
 * @param data The JsonBox Value to insert
 * @return Id of inserted value or 0 if error encountered
 *

std::string MongoInterface::insertUnitTests(std::string collection,
      JsonBox::Value data)
{
   std::string strReturn;
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return strReturn;
    }

   try {
      mongocxx::collection coll = dbc->m_db[collection];
      //std::cout << data << std::endl;
      if(data["submodules"].getObject().size() > 0){

         for(JsonBox::Object::const_iterator it = data["submodules"].getObject().begin(); 
               it !=data["submodules"].getObject().end(); ++it){
            std::string str = this->insertUnitTests(collection, data["submodules"][it->first]);
            JsonBox::Value json;
            json["_id"] = str;
            json["pass"] = data["submodules"][it->first]["pass"];
            data["submodules"][it->first] = json;
         }
      }
      strReturn = this->insert(collection, data);
   } catch (const mongocxx::bulk_write_exception& e) {
      std::cout << "insert: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "insert: default exception" << std::endl;
   }

   m_clients.enqueue(dbc);
   return strReturn;
}
*/

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
 **/
JsonBox::Value MongoInterface::query( std::string collection
                                    , JsonBox::Value data)
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return {};
    }

    JsonBox::Value results;
    bool success = false;
   try{
      mongocxx::collection coll = dbc->m_db[collection];
      mongocxx::cursor cursor = coll.find(BSON_from_JSON(data));
      int i = 0;
      for(auto doc : cursor) {
         results[i] = JSON_from_BSON(doc);
         i++;
      }
      success = true;
   } /*catch (const mongocxx::logic_error& e) {
      std::cout << "query: " << e.what() << std::endl;
   } */
   catch (const mongocxx::query_exception& e) {
      std::cout << "query returned empty" << std::endl;
   }
   catch (...) {
      std::cout << "query: default exception" << std::endl;
   }

   m_clients.enqueue(dbc);
   if( success ) return results;
   return {};
}
/**
 * Query the specified collection for all JsonBox Values
 *
 * @param collection The name of the collection to query
 * @return Array of results
 **/
JsonBox::Value MongoInterface::queryAll(std::string collection)
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return {};
    }

    JsonBox::Value results;
    bool success = false;
   try{
      mongocxx::collection coll = dbc->m_db[collection];
      mongocxx::cursor cursor = coll.find(document{} << finalize);
      int i = 0;
      for(auto doc : cursor) {
         results[i] = JSON_from_BSON(doc);
         i++;
      }
      success = true;
   } /*catch (const mongocxx::logic_error& e) {
      std::cout << "query: " << e.what() << std::endl;
   } */
   catch (const mongocxx::query_exception& e) {
      std::cout << "query returned empty" << std::endl;
   }
   catch (...) {
      std::cout << "queryAll: default exception" << std::endl;
   }

   m_clients.enqueue(dbc);
   if( success ) return results;
   return {};
}

/**
 * Remove one or multiple entries from the specified collection
 *
 * @param collection The name of the collection
 * @param data A JsonBox Value specifying what entries to remove
 * @return True on success
 **/
bool MongoInterface::removeEntry(std::string collection, JsonBox::Value data) 
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return false;
    }

    bool rc = false;
   try {
      mongocxx::collection coll = dbc->m_db[collection];
      coll.delete_many(BSON_from_JSON(data));
       rc = true;
   } catch (const mongocxx::bulk_write_exception& e) {
      std::cout << "removeEntry: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "removeEntry: default exception" << std::endl;
   }

   m_clients.enqueue(dbc);
   return rc;
}

/**
 * this queries a collection for the specified value, and updates it with
 * the passed parameters.  This function will only update one entry if the
 * query matches multiple.
 *
 * @param collection The name of the collection
 * @param filter Which entry to update
 * @param update The parameters to update with
 * @return True on success
 **/
bool MongoInterface::update(std::string collection, JsonBox::Value filter, JsonBox::Value update)
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return false;
    }

    bool rc = false;
   try{
      mongocxx::collection coll = dbc->m_db[collection];
      coll.update_many(BSON_from_JSON(filter), BSON_from_JSON(update));
      rc = true;
   } catch (const mongocxx::bulk_write_exception& e) {
      std::cout << "update: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "update: default exception" << std::endl;
   }

    m_clients.enqueue(dbc);
    return rc;
}

/**
 * Count the number of objects in the collection specified
 *
 * @param collection The name of the collection to query
 * @return Integer value of number
 */
int MongoInterface::count(std::string collection)
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return -1;
    }

    int rc = -1;
   try {
      mongocxx::collection coll = dbc->m_db[collection];
      rc = coll.count(document{} << finalize);
   } /*catch (const mongocxx::query_exception& e) {
      std::cout << "count: " << e.what() << std::endl;
   } */catch (...) {
      std::cout << "count: default exception" << std::endl;
   }

    m_clients.enqueue(dbc);
   return rc;
}

/**
 * Count the number of objects in the collection specified
 * that match the specified filter
 *
 * @param collection The name of the collection to query
 * @param filter The query used on collection  
 * @return Integer value of number
 **/
int MongoInterface::countFilter(std::string collection, JsonBox::Value filter)
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return -1;
    }

    int rc = -1;
   try {
      mongocxx::collection coll = dbc->m_db[collection];
      rc = coll.count(BSON_from_JSON(filter));
   } /*catch (const mongocxx::query_exception& e) {
      std::cout << "count: " << e.what() << std::endl;
   } */catch (...) {
      std::cout << "count: default exception" << std::endl;
   }

    m_clients.enqueue(dbc);
   return rc;
}

/**
 * Returns a string containing the IP and Port of the database
 *
 * @return The IP and port in the form "IP:Port"
 **/
std::string MongoInterface::getDatabase() const 
{
   return m_database;
}

/**
 * Returns a string containing the IP and Port of the database
 *
 * @return The IP and port in the form "IP:Port"
 */
std::string MongoInterface::getURI() const 
{
   return m_URI;
}

/**
 * \brief returns the maximum number of clients this instance can support
 **/
int MongoInterface::getMaxClients() const
{
   return m_maxClients;
}

/**
 * \brief Returns the number of clients that have been created.
 **/
int MongoInterface::getCreatedClients() const
{
   return m_createdClients;
}

/**
 * Remove all entries from the specified collection
 *
 * @param collection The name of the collection
 * @return True on success
 */
bool MongoInterface::removeAllEntries(std::string collection) 
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return false;
    }

    bool rc = false;
   try {
      mongocxx::collection coll = dbc->m_db[collection];
      coll.delete_many(document{} << finalize);
      rc = true;
   } catch (const mongocxx::bulk_write_exception& e) {
      std::cout << "removeAllEntries: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "removeAllEntries: default exception" << std::endl;
   }

    m_clients.enqueue(dbc);
   return rc;
}

/**
 * Drop specified collection
 *
 * @param collection The name of the collection
 * @return True on success
 **/
bool MongoInterface::dropCollection(std::string collection) 
{
    MongoDatabaseClientPtr dbc;
    if( !m_clients.dequeue(dbc, 10) ){
        std::cout << "FAILED TO GET MONGO DATABASE CLIENT" << std::endl;
        return false;
    }

    bool rc = false;
   try {
      mongocxx::collection coll = dbc->m_db[collection];
      coll.drop();
      rc = true;
   } catch (const mongocxx::operation_exception& e) {
      std::cout << "dropCollection: " << e.what() << std::endl;
   } catch (...) {
      std::cout << "dropCollection: default exception" << std::endl;
   }

   m_clients.enqueue(dbc);
   return rc;
}

/**
 * \brief Function to generate a MongoAPI reference from a MongoInterface object
 * \param [in] uri ip address/port of the mongodb instance to connect to. Default:127.0.0.1:27017
 * \return MongoAPI object 
 **/
std::shared_ptr<MongoAPI> getMongoDBInterface( std::string uri )
{
   return std::shared_ptr<MongoAPI> MongoInterface(uri);
}

} //end namespace mongoapi
