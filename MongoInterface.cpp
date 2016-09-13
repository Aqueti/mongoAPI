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
/*
MongoInterface::MongoInterface(std::string database, std::string IP_Port) {
        m_client{"mongo://localhost:2017"};
        db = conn["test"];
//	connection = new mongo::DBClientConnection();
//	connect(database, IP_Port);
}

MongoInterface::~MongoInterface() {
//	mongo::client::shutdown();
//	delete connection;
}
bool MongoInterface::connect(std::string database, std::string IP_Port) {
	mongo::client::initialize();
	try {
		connection->connect(IP_Port);
		std::cout << "connected ok" << std::endl;
	} catch (const mongo::DBException &e) {
		std::cout << "caught " << e.what() << std::endl;
		return false;
	}
	this->IP_Port = IP_Port;
	this->database = database;
	return true;
}

mongo::BSONObj MongoInterface::BSON_from_JSON(JsonBox::Value* data) const {
	std::stringstream stream;
	data->writeToStream(stream, false);
	return mongo::fromjson(stream.str());
}

JsonBox::Value MongoInterface::JSON_from_BSON(mongo::BSONObj* data) const {
	JsonBox::Value j;
	j.loadFromString(data->jsonString());
	return j;
}

bool MongoInterface::insertJSON(std::string collection,
		JsonBox::Value* data) const {
	try {
		connection->insert(database + "." + collection, BSON_from_JSON(data));
		return true;
	} catch (const mongo::DBException& e) {
		std::cout << "caught " << e.what() << std::endl;
	}
	return false;
}

JsonBox::Value MongoInterface::query(std::string collection,
		JsonBox::Value* data) const {
	JsonBox::Value results;
	std::auto_ptr<mongo::DBClientCursor> cursor = connection->query(
			database + "." + collection, BSON_from_JSON(data));
	int counter = 0;
	while (cursor->more()) {
		mongo::BSONObj b = cursor->next();
		results[counter] = JSON_from_BSON(&b);
		counter++;
	}
	return results;
}

bool MongoInterface::removeEntry(std::string collection, JsonBox::Value* data,
		bool onlyOne) {
	try {
		connection->remove(database + "." + collection, BSON_from_JSON(data),
				onlyOne);
	} catch (const mongo::DBException& e) {
		return false;
	}
	return true;
}

bool MongoInterface::update(std::string collection, JsonBox::Value* query,
			JsonBox::Value* update, bool onlyOne){

	try{
		connection->update(database + "." + collection, BSON_from_JSON(query), BSON_from_JSON(update), false, !onlyOne);
	}catch(const mongo::DBException& e){

	}

	return true;
}

std::string MongoInterface::getDatabase() const {
	return database;
}

void MongoInterface::setDatabase(std::string database) {
	this->database = database;
}

std::string MongoInterface::getIP_Port() const {
	return IP_Port;
}
*/
