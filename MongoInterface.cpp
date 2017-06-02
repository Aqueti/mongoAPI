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

#include "MongoInterface.h"

namespace atl 
{
	MongoInterface::MongoInterface(std::string database, std::string URI) {
		connect(database, URI);
	}

	MongoInterface::~MongoInterface() {
	}

	bool MongoInterface::connect(std::string database, std::string URI) {
		m_uri = mongocxx::uri("mongodb://" + URI);
		m_client = mongocxx::client(m_uri);
		m_db = m_client[database];
		this->m_URI = URI;
		this->m_database = database;
		return true;
	}

	bsoncxx::document::value MongoInterface::BSON_from_JSON(JsonBox::Value data){
		std::stringstream stream;
		data.writeToStream(stream, false);
		return bsoncxx::from_json(stream.str());
	}

	JsonBox::Value MongoInterface::JSON_from_BSON(bsoncxx::document::view data){
		JsonBox::Value j;
		j.loadFromString(bsoncxx::to_json(data));
		return j;
	}

	bool MongoInterface::insertJSON(std::string collection,
			JsonBox::Value data){
		try {
			mongocxx::collection coll = m_db[collection];
			coll.insert_one(BSON_from_JSON(data));
			return true;
		} catch (const mongocxx::bulk_write_exception& e) {
			std::cout << "insertJSON: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "insertJSON: default exception" << std::endl;
		}
		return false;
	}

	JsonBox::Value MongoInterface::query(std::string collection,
			JsonBox::Value data){
		try{
			JsonBox::Value results;
			mongocxx::collection coll = m_db[collection];
			mongocxx::cursor cursor = coll.find(BSON_from_JSON(data));
			int i = 0;
			for(auto doc : cursor) {
			  results[i] = JSON_from_BSON(doc);
			  i++;
			}
			return results;
		} /*catch (const mongocxx::logic_error& e) {
			std::cout << "query: " << e.what() << std::endl;
		} */catch (...) {
			std::cout << "query: default exception" << std::endl;
		}
	}

	bool MongoInterface::removeEntry(std::string collection, JsonBox::Value data) {
		try {
			mongocxx::collection coll = m_db[collection];
			coll.delete_many(BSON_from_JSON(data));
			return true;
		} catch (const mongocxx::bulk_write_exception& e) {
			std::cout << "removeEntry: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "removeEntry: default exception" << std::endl;
		}
		return false;
	}

	bool MongoInterface::update(std::string collection, JsonBox::Value filter, JsonBox::Value update){
		try{
			mongocxx::collection coll = m_db[collection];
			coll.update_many(BSON_from_JSON(filter), BSON_from_JSON(update));
			return true;
		} catch (const mongocxx::bulk_write_exception& e) {
			std::cout << "update: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "update: default exception" << std::endl;
		}
		return false;
	}

	int MongoInterface::count(std::string collection){
		try {
			mongocxx::collection coll = m_db[collection];
			return coll.count(document{} << finalize);
		} /*catch (const mongocxx::query_exception& e) {
			std::cout << "count: " << e.what() << std::endl;
		} */catch (...) {
			std::cout << "count: default exception" << std::endl;
		}
	}

	int MongoInterface::countFilter(std::string collection, JsonBox::Value filter){
		try {
			mongocxx::collection coll = m_db[collection];
			return coll.count(BSON_from_JSON(filter));
		} /*catch (const mongocxx::query_exception& e) {
			std::cout << "count: " << e.what() << std::endl;
		} */catch (...) {
			std::cout << "count: default exception" << std::endl;
		}
	}

	std::string MongoInterface::getDatabase() const {
		return m_database;
	}

	std::string MongoInterface::getURI() const {
		return m_URI;
	}

















	//specific for the unit tests - talk to steve about these

	JsonBox::Value MongoInterface::queryAll(std::string collection){
		try{
			JsonBox::Value results;
			mongocxx::collection coll = m_db[collection];
			mongocxx::cursor cursor = coll.find(document{} << finalize);
			int i = 0;
			for(auto doc : cursor) {
			  results[i] = JSON_from_BSON(doc);
			  i++;
			}
			return results;
		} /*catch (const mongocxx::logic_error& e) {
			std::cout << "query: " << e.what() << std::endl;
		} */catch (...) {
			std::cout << "query: default exception" << std::endl;
		}
	}

	bool MongoInterface::removeAllEntries(std::string collection) {
		try {
			mongocxx::collection coll = m_db[collection];
			coll.delete_many(document{} << finalize);
			return true;
		} catch (const mongocxx::bulk_write_exception& e) {
			std::cout << "removeAllEntries: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "removeAllEntries: default exception" << std::endl;
		}
		return false;
	}

	bool MongoInterface::dropCollection(std::string collection) {
		try {
			mongocxx::collection coll = m_db[collection];
			coll.drop();
			return true;
		} catch (const mongocxx::operation_exception& e) {
			std::cout << "dropCollection: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "dropCollection: default exception" << std::endl;
		}
		return false;
	}

	bool testMongoInterface(bool printFlag, bool asserFlag){
		try {
			//construct the interface object, defualt constructor creates connection to database
			//default parameters are database: "aqueti", URI: "127.0.0.1:27017"
			MongoInterface mi("aqueti");
			JsonBox::Value val1;
			JsonBox::Value val2;
			JsonBox::Value val3;
			JsonBox::Value val4;
			JsonBox::Value val5;
			JsonBox::Value val6;
			JsonBox::Value val7;
			JsonBox::Value val8;
			JsonBox::Value val9;
			JsonBox::Value val10;
			JsonBox::Value result;
			int count;

			//test values for insert
			val1["name"] = "erik";
			val1["class"] = "senior";
			val1["gender"] = "male";
			val2["name"] = "alex";
			val2["class"] = "sophomore";
			val2["gender"] = "male";
			val3["name"] = "emma";
			val3["class"] = "freshman";
			val3["gender"] = "female";
			val4["name"] = "emma";
			val4["class"] = "junior";
			val4["gender"] = "female";

			//test value for query and remove
			val5["name"] = "emma";
			val8["name"] = "erik";
			val9["name"] = "alex";
			val10["$or"][(size_t)0] = val8;
			val10["$or"][1] = val9;

			//test value for update 
			val6["name"] = "erik";
			val7["$set"]["class"] = "graduate";

			//drop collection before testing for a clean test environment
			mi.dropCollection("test");

			//test insert function of database
			//will create test collection if it does not already exist
			mi.insertJSON("test", val1);
			mi.insertJSON("test", val2);
			mi.insertJSON("test", val3);
			mi.insertJSON("test", val4);

			count = mi.count("test");
			if(count != 4){
				if(printFlag){
					std::cout << "incorrect number of values inserted: " + count << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			//test query function of database
			result = mi.query("test", val5);

			count = result.getArray().size();
			if(count != 2){
				if(printFlag){
					std::cout << "query attempt 1 failed; results expected: 2, received: " + count << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			result = mi.query("test", val10);

			count = result.getArray().size();
			if(count != 2){
				if(printFlag){
					std::cout << "query attempt 2 failed; results expected: 2, received: " + count << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			//test remove function of database
			mi.removeEntry("test", val5);

			count = mi.countFilter("test", val5);
			if(count != 0){
				if(printFlag){
					std::cout << "remove attempt failed, values matching filter still present" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			//test update function of database
			mi.update("test", val6, val7);
			result = mi.query("test", val6);

			if(result[(size_t)0]["class"] != "graduate"){
				if(printFlag){
					std::cout << "update attempt failed" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

		} catch (const std::exception& ex) {
			std::cout << ex.what() << std::endl;
			return 0;
		} catch (const std::string& ex) {
			std::cout << ex << std::endl;
			return 0;
		} catch (...) {
			std::cout << "default exception";
			return 0;
		}
		return 1;
	}	
}

