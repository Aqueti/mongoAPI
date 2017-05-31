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

MongoInterface::MongoInterface(std::string database, std::string IP_Port) {
	connect(database, IP_Port);
}

MongoInterface::~MongoInterface() {
}
bool MongoInterface::connect(std::string database, std::string IP_Port) {
	db = client[database];
	/*
	try {
		connection.connect(IP_Port);
		std::cout << "connected ok" << std::endl;
	} catch (const mongo::DBException &e) {
		std::cout << "caught " << e.what() << std::endl;
		return false;
	}
	*/
	this->IP_Port = IP_Port;
	this->database = database;
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
		mongocxx::collection coll = db[collection];
		mongocxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(BSON_from_JSON(data));
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
	JsonBox::Value results;
	mongocxx::collection coll = db[collection];
	mongocxx::cursor cursor = coll.find(BSON_from_JSON(data));
	int i = 0;
	for(auto doc : cursor) {
	  results[i] = JSON_from_BSON(doc);
	  i++;
	}
	return results;
}

bool MongoInterface::removeEntry(std::string collection, JsonBox::Value data) {
	try {
		mongocxx::collection coll = db[collection];
		coll.delete_many(BSON_from_JSON(data));
		return true;
	} catch (const mongocxx::bulk_write_exception& e) {
		std::cout << "removeEntry: " << e.what() << std::endl;
	} catch (...) {
		std::cout << "removeEntry: default exception" << std::endl;
	}
	return false;
}

/*
bool MongoInterface::update(std::string collection, JsonBox::Value query,
			JsonBox::Value update, bool onlyOne){

	try{
		connection.update(database + "." + collection, BSON_from_JSON(query), BSON_from_JSON(update), false, !onlyOne);
	}catch(const mongo::DBException& e){

	}

	return true;
}
*/
std::string MongoInterface::getDatabase() const {
	return database;
}

void MongoInterface::setDatabase(std::string database) {
	this->database = database;
	//probably should reset the connection here
	//connect(database, IP_Port);
}

std::string MongoInterface::getIP_Port() const {
	return IP_Port;
}