/**
 * \file MongoAPITest.cpp
 *
 * Created on: June 19, 2017
 * Author: Nils Persson
 **/

#include "MongoAPITest.h"

namespace mongoapi {

/**
 * \brief Run tests on all components in the mongoapi library
 **/
JsonBox::Value testMongoAPI(std::string uri, bool testSubmodules, bool printFlag, bool assertFlag, std::vector<std::string> unitList) {
	//define variables used
	JsonBox::Value jsonReturn;
	JsonBox::Value jsonUnits;
	JsonBox::Value jsonSub;
	JsonBox::Value jsonValue;
	bool pass = true;

	//get type
	jsonReturn["type"] = "unit_tests";

	//get repository
	jsonReturn["component"] = "mongoAPI";

	//get timestamp
	std::time_t time = std::time(NULL);
	char mbstr[100];
	if (std::strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&time))) {
        jsonReturn["date"] = mbstr;
    }

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
	for(std::vector<std::string>::iterator it = unitList.begin(); it != unitList.end(); ++it ) {
		if( !it->compare("MongoInterface")) {
			std::cout << "Testing MongoInterface..." << std::endl;
			jsonValue = mongoapi::testMongoInterface(uri, printFlag, assertFlag);
		    jsonUnits["MongoInterface"] = jsonValue;
		    jsonReturn["units"] = jsonUnits;
		    if(jsonValue["pass"].getBoolean()){
		        std::cout << "MongoInterface passed successfully!" << std::endl;
		        pass = pass && true;
		    }
		    else{
		        std::cout << "MongoInterface failed to pass!" << std::endl;
		        pass = pass && false;
		    }
		}
	}

    //get test results from all submodules
    // if(testSubmodules) {
    // 	std::cout << "Testing AquetiTools..." << std::endl;
    //     jsonValue = testAquetiTools();
    //     jsonSub["AquetiTools"] = jsonValue;
    //     jsonReturn["submodules"] = jsonUnits;
    //     if(jsonValue["pass"].getBoolean()){
	   //      std::cout << "AquetiTools passed successfully!" << std::endl;
	   //      pass = pass && true;
	   //  }
	   //  else{
	   //      std::cout << "AquetiTools failed to pass!" << std::endl;
	   //      pass = pass && false;
	   //  }
    // }
	
	//get pass
    jsonReturn["pass"] = pass;
    return jsonReturn;
}
}