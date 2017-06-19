/*
 * MongoInterface_test.cpp
 *
 *  Created on: June 19, 2017
 *      Author: Nils Persson
 */

#include "mongoapi.h"

/**
 * \brief Run tests on all components in the mongoapi library
 **/
JsonBox::Value testMongoAPI( bool testSubmodules = true)
{
	JsonBox::Value jsonReturn;
	JsonBox::Value jsonValue;
	std::string jsonString = mongoapi::testMongoInterface(true, false);
	jsonValue.loadFromString(jsonString);
    jsonReturn["mongoInterface"] = jsonValue;

    /*
    if( testSubmodules) {
       value["submodule"]["AquetiTools"] = atl::testAquetiTools();
    }
	*/
    return jsonReturn;
}

int main(int argc, char *argv[]) {
	//call test function for mongoAPI
	JsonBox::Value result = testMongoAPI();

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
			mi.insertJSON("unit_tests", result);
		}
		else{
			std::cout << "failed to insert unit test results" << std::endl;
			return 0;
		}
	}

}
