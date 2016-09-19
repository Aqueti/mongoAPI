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

	    //mongocxx::instance inst{bsoncxx::stdx::make_unique<logger>(&std::cout)};


	    try {

	    	// create uri format
		    std::string port_string = std::to_string(port);
		    std::string buf("mongodb://");
		    buf.append(uris);
		    buf.append(":");
		    buf.append(port_string);

		    // create uri
	        const auto uri = mongocxx::uri{buf};

	        // generate client options
	        mongocxx::options::client client_options;

	        // if (uri.ssl()) {
	        //     mongocxx::options::ssl ssl_options;
	        //     // NOTE: To test SSL, you may need to set options. The following
	        //     // would enable certificates for Homebrew OpenSSL on OS X.
	        //     // options.ca_file("/usr/local/etc/openssl/cert.pem");
	        //     // ssl_options.ca_file("/usr/local/etc/openssl/cert.pem");
	        //     client_options.ssl_opts(ssl_options);
	        // }

	        // create client object
	        m_conn = mongocxx::client{uri,client_options};
	        m_db = database;
	        auto admin = m_conn[m_db];


	        document ismaster;
	        ismaster << "isMaster" << 1;

	        // view document
	        auto result = admin.run_command(ismaster.view());

	        //prints out master document
	        // std::cout << bsoncxx::to_json(result) << "\n";

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
   		auto db = m_conn[m_db];

   		try{
	   		bsoncxx::document::value newData = BSON_from_JSON(data);

			auto result = db[collection].insert_one(newData.view());


			 // auto res = db[collection].insert_one(std::move(newData));

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
			//not sure what to use for restaurants
	        auto cursor = db[collection].find({});
	        // need to make this return
	        for (auto&& doc : cursor) {
	            std::cout << bsoncxx::to_json(doc) << std::endl;
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
       mongoAPI::MongoInterface interface( "test", "localhost", 27014 );

       JsonBox::Value dbinfo = interface.getDBInfo();

       JsonBox::Value value("{\"id\":1234, \"name\":\"test1\"}");

       interface.insertJSON( "test", value);
       
       return true;
   }

}
