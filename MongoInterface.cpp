/*
 * Interface.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: Cameron Givler
 */

#include "MongoInterface.h"

MongoInterface::MongoInterface() {
	connection = new mongo::DBClientConnection();
	connect();
}

MongoInterface::~MongoInterface() {
	delete connection;
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

bool MongoInterface::insertJSON(std::string collection, JsonBox::Value* data) const {
    try {
        connection->insert(database + "." + collection, BSON_from_JSON(data));
        return true;
    } catch (const mongo::DBException& e) {
        std::cout << "caught " << e.what() << std::endl;
    }
    return false;
}

JsonBox::Value MongoInterface::query(std::string collection, JsonBox::Value* data) const {
    JsonBox::Value results;
    std::auto_ptr<mongo::DBClientCursor> cursor = connection->query(database + "." + collection, BSON_from_JSON(data));
    int counter = 0;
    while (cursor->more()) {
        mongo::BSONObj b = cursor->next();
        results[counter] = JSON_from_BSON(&b);
        counter++;
    }
    results.writeToStream(std::cout);
    return results;
}

bool MongoInterface::removeEntry(std::string collection, JsonBox::Value* data, bool onlyOne){
	try{
		connection->remove(database + "." + collection, BSON_from_JSON(data), onlyOne);
	} catch(const mongo::DBException& e){
		return false;
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

int main(){
    MongoInterface mi;
    JsonBox::Value val1;
    val1["TestKey"] = JsonBox::Value("TestVal");
    mi.insertJSON("Test", &val1);
    JsonBox::Value val2;
    val2["TestKey"]["$in"][size_t(0)] = JsonBox::Value("TestVal");
    val2["TestKey"]["$in"][1] = JsonBox::Value("TestVal2");
    std::cout << val2 << std::endl;
    mi.query("Test", &val2);
    mi.removeEntry("Test", &val1, false);
    mi.query("test", &val2);
    return true;
}

