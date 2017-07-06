
/**
 * \file testMongoAPI.cpp
 *
 * Created on: June 19, 2017
 * Author: Nils Persson
 **/

#include "MongoAPITest.h"

std::vector<std::string> unitList{"MongoInterface"};

int main(int argc, char *argv[]) {

	bool testSubmodules = true;
	bool testAll = true;
	bool insert = true;

	//if the command line option is used then do not insert 
	int i;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-n") == 0){
			insert = false;
		} else if(!strcmp(argv[i], "-u")) {
            if (testAll) {
                unitList.clear();
                testAll = false;
            }
            i++;
            unitList.push_back( argv[i]);
        } else if(strcmp(argv[i], "-s") == 0){
			testSubmodules = false;
		}
	}

	//run tests
	std::cout << "Testing mongoAPI..." << std::endl;
	JsonBox::Value result = mongoapi::testMongoAPI(unitList, testSubmodules);
	if(result["pass"] == true){
		std::cout << "mongoAPI passed successfully!" << std::endl;
	}
	else{
		std::cout << "mongoAPI failed to pass!" << std::endl;
	}

	//connect to database and insert JsonValue if "-n" was not used
	if(insert){
		std::cout << "Inserting unit test results in database..." << std::endl;
		mongoapi::MongoInterface mi;
		bool connected = mi.connect("aqueti");
		if(connected){
			std::string id = mi.insertUnitTests("unit_tests", result);
			if(id != "0"){
				std::cout << "Results inserted in database successfully!" << std::endl;
			}
		}
		else{
			std::cout << "Failed to insert unit test results!" << std::endl;
			return 0;
		}
	}

	return 0;
}