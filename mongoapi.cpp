/**
 * @file mongoapi.h
 * @author Nils Persson <npersson@live.unc.edu>
 * @version 1.0
 *
 * Interface class to MongoDB
 *
 * A class to connect to a MongoDB database with methods to insert, retrieve,
 * and remove entries with JsonBox Values.
 */

#include "mongoapi.h"

namespace mongoapi
{
	MongoInterface::MongoInterface() {
	}

	MongoInterface::~MongoInterface() {
	}

	bool MongoInterface::connect(std::string database, std::string URI) {
		try{
			m_uri = mongocxx::uri("mongodb://" + URI + "");
			m_client = mongocxx::client(m_uri);
			m_db = m_client[database];

			JsonBox::Value val;
			val["ismaster"] = 1;
			auto result = m_db.run_command(BSON_from_JSON(val));
		}
		catch(const mongocxx::exception& e){
			std::cout << "database connection failed" << std::endl;
			return false;
		}
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

	std::string MongoInterface::insert(std::string collection,
			JsonBox::Value data){
		std::string strReturn;
		try {
			mongocxx::collection coll = m_db[collection];
 			mongocxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(BSON_from_JSON(data));
 			if(result){
 				bsoncxx::types::value id = result->inserted_id();
 				if(id.type() == bsoncxx::type::k_oid){
 					bsoncxx::oid oid = id.get_oid().value;
 					strReturn = oid.to_string();
 					return strReturn;
 				}
 			}
		} catch (const mongocxx::bulk_write_exception& e) {
			std::cout << "insert: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "insert: default exception" << std::endl;
		}
		return strReturn;
	}

	std::string MongoInterface::insertUnitTests(std::string collection,
			JsonBox::Value data){
		std::string strReturn;
		try {
			mongocxx::collection coll = m_db[collection];
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
			return strReturn;
		} catch (const mongocxx::bulk_write_exception& e) {
			std::cout << "insert: " << e.what() << std::endl;
		} catch (...) {
			std::cout << "insert: default exception" << std::endl;
		}
		return strReturn;
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
		} */
		catch (const mongocxx::query_exception& e) {
			std::cout << "query returned empty" << std::endl;
		}
		catch (...) {
			std::cout << "query: default exception" << std::endl;
		}
		return 0;
	}

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
		} */
		catch (const mongocxx::query_exception& e) {
			std::cout << "query returned empty" << std::endl;
		}
		catch (...) {
			std::cout << "queryAll: default exception" << std::endl;
		}
		return 0;
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
		return -1;
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
		return -1;
	}

	std::string MongoInterface::getDatabase() const {
		return m_database;
	}

	std::string MongoInterface::getURI() const {
		return m_URI;
	}

	/****************************************************************************/
	/************************** USED FOR TESTING ONLY ***************************/
	/****************************************************************************/

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
}

