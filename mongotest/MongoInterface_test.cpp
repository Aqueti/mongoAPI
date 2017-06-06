/*
 * MongoInterface_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: cameron
 */

#include "mongoapi.h"

int main() {
	//call unit test function and store result in JsonValue
	std::string jsonString = mongoapi::testMongoInterface(true, false);
	JsonBox::Value jsonVal;
	jsonVal.loadFromString(jsonString);
	std::cout << jsonVal << std::endl;

	//connect to database and insert JsonValue
	mongoapi::MongoInterface mi("aqueti");
	mi.insertJSON("unit_tests", jsonVal);

    

}
