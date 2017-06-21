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
	//define variables used
	JsonBox::Value jsonReturn;
	JsonBox::Value jsonUnits;
	JsonBox::Value jsonValue;

	//get type
	jsonReturn["type"] = "unit_tests";

	//get repository
	jsonReturn["component"] = "mongoAPI";

	//get timestamp
	jsonReturn["date"] = aqt::getDateAsString();

	//get /etc/quid
	std::string guid;
	std::ifstream nameFileout;
	nameFileout.open("/etc/guid");
	if(nameFileout.good()){
		getline(nameFileout, guid);
	}
	nameFileout.close();
	jsonReturn["hardwareId"] = guid;

    //get commit hash id and version
	jsonReturn["commit"] = mongoapi::GIT_COMMIT_HASH;
	jsonReturn["version"] = mongoapi::VERSION;
	std::string softwareId1 = mongoapi::VERSION;
	std::string softwareId2 = mongoapi::GIT_COMMIT_HASH;
	std::string softwareId = softwareId1 + ":" + softwareId2;
	jsonReturn["softwareId"] = softwareId;

	//get test results from all classes (units)
	std::string jsonString = mongoapi::testMongoInterface(true, false);
	jsonValue.loadFromString(jsonString);
    jsonUnits["MongoInterface"] = jsonValue;
    jsonReturn["units"] = jsonUnits;

    /*
    if( testSubmodules) {
       value["submodules"]["AquetiTools"] = atl::testAquetiTools();
    }
	*/

	//get pass
    jsonReturn["pass"] = false;

    return jsonReturn;
}

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
