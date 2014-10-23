/* 
 * File:   MongoInterface.h
 * Author: cameron
 *
 * Created on October 21, 2014, 3:54 PM
 */

#ifndef MONGOINTERFACE_H
#define	MONGOINTERFACE_H

#include <cstdlib>
#include <iostream>
#include <JsonBox.h>
#include "mongo/client/dbclient.h"

class MongoInterface {
private:
    std::string database;
    std::string IP_Port;
    mongo::DBClientConnection* connection;
    mongo::BSONObj BSON_from_JSON(JsonBox::Value* data) const;
    JsonBox::Value JSON_from_BSON(mongo::BSONObj* data) const;
    
public:
    MongoInterface();
    MongoInterface(const MongoInterface& orig);
    virtual ~MongoInterface();
    bool connect(std::string IP_Port="localhost", std::string database="aqueti");
    bool insertJSON(std::string collection, JsonBox::Value* data) const;
    JsonBox::Value query(std::string collection, JsonBox::Value* data) const;
    std::string getDatabase() const;
    void setDatabase(std::string database);
    std::string getIP_Port() const;
};

#endif	/* MONGOINTERFACE_H */

