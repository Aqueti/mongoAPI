/*
 * Interface.h
 *
 *  Created on: Nov 6, 2014
 *      Author: Cameron Givler
 */

#ifndef MONGOINTERFACE_H_
#define MONGOINTERFACE_H_

#include <cstdlib>
#include <iostream>
#include "JsonBox.h"
#include "mongo/client/dbclient.h"
#include "mongo/bson/bson.h"

class MongoInterface {
private:
    std::string database;
    std::string IP_Port;
    mongo::DBClientConnection* connection;
    mongo::BSONObj BSON_from_JSON(JsonBox::Value* data) const;
    JsonBox::Value JSON_from_BSON(mongo::BSONObj* data) const;
public:
    MongoInterface();
    virtual ~MongoInterface();
    bool connect(std::string database="aqueti", std::string IP_Port="localhost");
    bool insertJSON(std::string collection, JsonBox::Value* data) const;
    JsonBox::Value query(std::string collection, JsonBox::Value* data) const;
    bool removeEntry(std::string collection, JsonBox::Value* data, bool onlyOne);
    std::string getDatabase() const;
    void setDatabase(std::string database);
    std::string getIP_Port() const;
};

bool mongoInterfaceTest();

#endif /* MONGOINTERFACE_H_ */
