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
#include <sstream>

#include "MongoInterface.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

namespace mongoAPI
{

   /**
    * \brief Constructor
    * \param [in] database name of the database to connect to
    * \param [in] uri address of machine hosting database
    * \param [in] port database listening port
    *
    * If the input strings are not specified, do not connect. 
    **/ 
   MongoInterface::MongoInterface(std::string database, std::string uri, size_t port) 
   {
        if( !database.empty() && !uri.empty() && port ) {
        connect( database, uri, port)
   }


   /**
    * \brief Destructor
    **/
   MongoInterface::~MongoInterface() 
   {
   }

   /**
    * \brief Establishes a connection with a database
    * \param [in] uri 
    * \return true on success, false on failure
    **/
   bool MongoInterface::connect(std::string database, std::string uri, size_t port ) 
   {
        //Connect to the server

	return true;
   }

   /**
   * \brief Converts a JSON string into a BSON object
   * \return bsoncxx::document::value of the JsonBox data
   **/
   bsoncxx::document::value MongoInterface::BSON_from_JSON(JsonBox::Value data) 
   {
       std::stringstream ss;

       bsoncxx::builder::stream::document doc{};
       ss<<data;
       doc << ss.str();

       return doc.extract();
   }

   /**
    * Helper method to convert a BSON object from MongoDB to a JsonBox Value
    *
    * \param [in] data the BSON object to convert
    * \return the JsonBox Value
    */
   JsonBox::Value MongoInterface::JSON_from_BSON(bsoncxx::document::value data) {
	JsonBox::Value j;
	j.loadFromString(bsoncxx::to_json(data.view()));
	return j;
   }

   /**
    * Insert a JsonBox Value into the database with specified collection.
    *
    * \param [in] collection The name of the collection to insert Value into
    * \param [in] data The JsonBox Value to insert
    * \return true on success
    **/
   bool MongoInterface::insertJSON(std::string collection, JsonBox::Value & data) 
   {
	return false;
   }


   /**
    * Query the specified collection according to a specified JsonBox Value
    *
    * \param [in] collection The name of the collection to query
    * \param [in] data The JsonBox Value query
    * \return Array of results
    **/
   JsonBox::Value MongoInterface::query(std::string collection, JsonBox::Value & data) 
   {
	JsonBox::Value results;
	return results;
   }

   /**
    * Remove one or multiple entries from the specified collection
    *
    * \param [in] collection The name of the collection
    * \param [in] data A JsonBox Value specifying what entries to remove
    * \param [in] onlyOne If true, a maximum of one entry will be removed
    * \return true on success
    **/
   bool MongoInterface::removeEntry(std::string collection, JsonBox::Value & data, bool onlyOne) 
   {
	return true;
   }

   /**
    * \brief this queries a collection for the specified value, and updates it with
    * the passed parameters. 
    * query matches multiple.
    *
    * \param [in] collection The name of the collection
    * \param [in] query Which entry to update
    * \param [in] update The parameters to update with
    * \return true on success
    *
    * This function will only update one entry if the
    **/
   bool MongoInterface::update( std::string collection
                              , JsonBox::Value & query
                              , JsonBox::Value & update
                              , bool onlyOne)
   {
	return true;
   }

   /**
    * Returns a string containing the name of the current database
    *
    * \return The name of the current database
    **/
   std::string MongoInterface::getDatabase() 
   {
      return database;
   }


   /**
    * \brief returns the uri, port and database 
    * \return string that includes the uri, port and database or empty string if not connected
    **/
   std::string MongoInterface::getIP_Port() 
   {
      std::string result;
	return result;
   }
}
