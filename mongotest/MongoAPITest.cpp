/*
 * MongoInterface_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: cameron
 */

#include "mongoapi.h"

/**
 * \brief Run tests on all components in the mongoapi library
 **/
JsonBox::Value testMongoAPI( bool testSubmodules = true)
{
	JsonBox::Value value;
    value["mongoInterface"] = testMongoInterface();

    if( testSubmodules) {
       value["submodule"]["AquetiTools"] = atl::testAquetiTools();
    }

    return value;
}


int main(int argc, char *argv[]) {
	//call unit test function and store result in JsonValue
//	std::string jsonString = mongoapi::testMongoInterface(true, false);
//	JsonBox::Value jsonVal;
//	jsonVal.loadFromString(jsonString);

	jsonBox::Value result = testMongoAPI();



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
		mongoapi::MongoInterface mi;
		bool connected = mi.connect("aqueti");
		if(connected){
			mi.insertJSON("unit_tests", jsonVal);
		}
		else{
			std::cout << "failed to insert unit test results" << std::endl;
			return 0;
		}
	}

}
