/*
 * MongoInterface_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: cameron
 */

#include "mongoapi.h"

int main(int argc, char *argv[]) {
	//call unit test function and store result in JsonValue
	std::string jsonString = mongoapi::testMongoInterface(true, false);
	JsonBox::Value jsonVal;
	jsonVal.loadFromString(jsonString);
	//if the command line option is used then do not insert 
	bool insert = true;
	int i;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-n") == 0){
			insert = false;
		}
	}

	//connect to database and insert JsonValue if "-n" was not used
	if(insert){
		mongoapi::MongoInterface mi("aqueti");
		mi.insertJSON("unit_tests", jsonVal);
	}
}
