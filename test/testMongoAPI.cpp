/*
 * MongoInterface_test.cpp
 *
 *  Created on: June 19, 2017
 *      Author: Nils Persson
 */

#include "MongoAPITest.h"

int main(int argc, char *argv[]) {
	//call test function for mongoAPI
	JsonBox::Value result = testMongoAPI(false);

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
		//	mi.insert("unit_tests", result);
			mi.insertUnitTests("unit_tests", result);
		}
		else{
			std::cout << "failed to insert unit test results" << std::endl;
			return 0;
		}
	}
}