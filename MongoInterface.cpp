/* 
 * File:   MongoInterface.cpp
 * Author: cameron
 * 
 * Created on October 21, 2014, 3:54 PM
 */

#include "MongoInterface.h"

MongoInterface::MongoInterface() {
}

MongoInterface::MongoInterface(const MongoInterface& orig) {
}

MongoInterface::~MongoInterface() {
}

bool MongoInterface::connect(std::string IP_Port, std::string database) {
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
        results["results"][counter] = JSON_from_BSON(&b);
    }
    results.writeToStream(std::cout);
    return results;
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

bool mongoInterfaceTest(){
    MongoInterface mi;
    mi.connect();
    JsonBox::Value val;
    val["TestKey"] = JsonBox::Value("TestVal");
    mi.insertJSON("Test", &val);
    return true;
}