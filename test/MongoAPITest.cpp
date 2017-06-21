/*
 * MongoInterface_test.cpp
 *
 *  Created on: June 19, 2017
 *      Author: Nils Persson
 */

#include "MongoAPITest.h"

/**
 * \brief Run tests on all components in the mongoapi library
 **/
JsonBox::Value testMongoAPI( bool testSubmodules) {
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