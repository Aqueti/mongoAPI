/**
 * @file MongoInterface.cpp
 * @author Cameron Givler <cameron.givler@duke.edu>
 * @version 1.0
 *
 * Interface class to MongoDB
 *
 * A class to connect to a MongoDB database with methods to insert, retrieve,
 * and remove entries with JsonBox Values.
 */
#include <sstream>

#include <JsonBox.h>
#include "MongoInterface.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

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
        connect( database, uri, port);
      }
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
   bool MongoInterface::connect(std::string database, std::string uris, size_t port ) 
   {
	 using bsoncxx::builder::stream::document;

	    try {
	    	// create uri format
		    std::string port_string = std::to_string(port);
		    std::string buf("mongodb://");
		    buf.append(uris);
		    buf.append(":");
		    buf.append(port_string);

		    // create DB info
			  (m_dbInfo)["uri"] = JsonBox::Value(uris);
			  (m_dbInfo)["port"] = JsonBox::Value(port_string);
			  (m_dbInfo)["database"] = JsonBox::Value(database);

		    // create uri
	        const auto uri = mongocxx::uri{buf};

	        // generate client options
	        mongocxx::options::client client_options;

	        // create client object
	        m_conn = mongocxx::client{uri,client_options};
	        m_db = database;
	        auto admin = m_conn[m_db];


	        document ismaster;
	        ismaster << "isMaster" << 1;

	        // view document
	        auto result = admin.run_command(ismaster.view());

	        //prints out master document
	        std::cout << JSON_from_BSON(result) << "\n";

	        return EXIT_SUCCESS;

	    } catch (const std::exception& xcp) {
	        std::cout << "connection failed: " << xcp.what() << "\n";
	        return EXIT_FAILURE;
	    }
   }

   /**
   * \brief Converts a JSON string into a BSON object
   * \return bsoncxx::document::value of the JsonBox data
   **/
   bsoncxx::document::view MongoInterface::BSON_from_JSON(JsonBox::Value data) 
   {
   	   std::stringstream ss;
       ss<<data;
       return bsoncxx::from_json(ss.str());
   }

   /**
    * Helper method to convert a BSON object from MongoDB to a JsonBox Value
    *
    * \param [in] data the BSON object to convert
    * \return the JsonBox Value
    */
   JsonBox::Value MongoInterface::JSON_from_BSON(bsoncxx::document::view data) {
	JsonBox::Value j;
	j.loadFromString(bsoncxx::to_json(data));
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
   		auto db = m_conn[m_db];

   		try{
        	std::stringstream ss;
        	ss<<data;
			auto result = db[collection].insert_one(bsoncxx::from_json(ss.str()));
			return EXIT_SUCCESS;

		}catch (const std::exception& xcp) {
	        std::cout << "write failed: " << xcp.what() << "\n";
	        return EXIT_FAILURE;
	    }

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

		auto db = m_conn[m_db];

		try{
      std::stringstream ss;
      ss<<data;
	    auto cursor = db[collection].find(bsoncxx::from_json(ss.str()));

	    // add values to jsonbox
	    for (auto&& doc : cursor) {
	      return JSON_from_BSON(doc);
			 }
       return EXIT_SUCCESS;
		}catch (const std::exception& xcp) {
	        std::cout << "query failed: " << xcp.what() << "\n";
	        return EXIT_FAILURE;
	    }
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
	  	auto db = m_conn[m_db];

		try{
      std::stringstream ss;
      ss<<data;

			if (onlyOne == true){
				db[collection].delete_one(bsoncxx::from_json(ss.str()));
				return true;
			} else if(onlyOne == false){
				db[collection].delete_many(bsoncxx::from_json(ss.str()));
				return true;
			}
			return false;

		}catch (const std::exception& xcp) {
	        std::cout << "remove failed: " << xcp.what() << "\n";
	        return EXIT_FAILURE;
	    }
   }

   /**
    * \brief returns the uri, port and database 
    * \return JsonBox value that includes the uri, port and database name. Empty if not connected
    *
    * {
    *     "uri":"10.0.0.110",
    *     "port":9000,
    *     "name":"test"
    * }
    **/
   JsonBox::Value MongoInterface::getDBInfo() 
   {
	  return m_dbInfo;
   }

   /**
    * \brief unit test function
    **/ 
   bool testMongoInterface()
   {

   		try{

   		std::cout << "Initializing a connection:" << std::endl;	
   		std::cout << "" << std::endl;	

   		// initializes a connection
	    MongoInterface interface( "test", "10.0.0.160", 27017 );

	    // gets database information
	    std::cout << "database information:" << std::endl;	
	    JsonBox::Value dbinfo = interface.getDBInfo();
        std::cout << dbinfo << std::endl;  


	    // inserts a value
	    std::cout << "Insert Val1:" << std::endl;	
  		JsonBox::Value val1 = new JsonBox::Value();
  		(val1)["TestKey"] = JsonBox::Value("TestVal");
  		interface.insertJSON("test", val1);

  		// inserts another value
     	std::cout << "Insert Val2:" << std::endl;			
  		JsonBox::Value val2 = new JsonBox::Value();
  		(val2)["NextValue"] = JsonBox::Value("BlahBlah");
  		interface.insertJSON("test", val2);

  		//insets another value (different format)
     	std::cout << "Insert Val3:" << std::endl;	
  		JsonBox::Value val3 = new JsonBox::Value();
  		(val3)["1234"] = JsonBox::Value("test1");
  		interface.insertJSON("test", val3);


  		// query val1 checks if its there
  		std::cout << "Query Val1:" << std::endl;	
  		std::cout << interface.query("test", val1) << std::endl;

  		// qery val2 checks if its there
     	std::cout << "Query Val2:" << std::endl;	
  		std::cout << interface.query("test", val2) << std::endl;

  		// qery val3 checks if its there
     	std::cout << "Query Val3:" << std::endl;	
  		std::cout << interface.query("test", val3) << std::endl;

  		// remove entry
     	std::cout << "Remove Val1:" << std::endl;	
  		interface.removeEntry("test", val1, false);
      std::cout << "show its gone:" << std::endl;
  		std::cout << interface.query("test", val1) << std::endl;

      } catch (const std::exception& ex) {
		    std::cout << ex.what() << std::endl;
		    return 1;
   		}
   		return 0;
	  }
}
