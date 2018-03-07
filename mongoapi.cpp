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

mongocxx::instance MongoInterface::m_instance{};

MongoInterface::MongoInterface(std::string URI, int maxClients) : 
    m_uri("mongodb://" + URI + "/?minPoolSize=1&maxPoolSize=" + std::to_string(maxClients+1))
    , m_pool(m_uri)
{
	this->m_URI = URI;
	m_maxClients = maxClients;
	m_createdClients = 0;
}

MongoInterface::~MongoInterface()
{
}

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

bsoncxx::document::value MongoInterface::BSON_from_JSON(JsonBox::Value data)
{
	if(data.isNull()){
		return bsoncxx::from_json("{}");
	}
	std::stringstream stream;
	data.writeToStream(stream, false);
	return bsoncxx::from_json(stream.str());
}

JsonBox::Value MongoInterface::JSON_from_BSON(bsoncxx::document::view data)
{
	JsonBox::Value j;
	j.loadFromString(bsoncxx::to_json(data));
	return j;
}

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
		std::cout << "insert: " << e.what() << std::endl;
	} catch (...) {
		std::cout << "insert: default exception" << std::endl;
	}

    m_clients.enqueue(dbc);
	return strReturn;
}

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
			//	bsoncxx::oid oid = id.get_oid().value;
			//	strReturn = oid.to_string();
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

JsonBox::Value MongoInterface::query(std::string collection,
		JsonBox::Value data)
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


bool MongoInterface::removeEntries(std::string collection, JsonBox::Value data) 
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

int MongoInterface::count(std::string collection, JsonBox::Value filter)
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

std::string MongoInterface::getDatabase() const 
{
	return m_database;
}

std::string MongoInterface::getURI() const 
{
	return m_URI;
}

int MongoInterface::getMaxClients() const
{
	return m_maxClients;
}

int MongoInterface::getCreatedClients() const
{
	return m_createdClients;
}



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

} //end namespace mongoapi
