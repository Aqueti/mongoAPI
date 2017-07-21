
/**
 * \file testMongoAPI.cpp
 *
 * Created on: June 19, 2017
 * Author: Nils Persson
 **/

#include "MongoAPITest.h"

std::vector<std::string> unitList{"MongoInterface"};

/**
 * \brief prints out help to user
 **/
bool printHelp(void)
{
    std::cout << "testMongoAPI" << std::endl;
    //AquetiTool::printVersion();
    std::cout << std::endl;
    std::cout << "Usage: ./testMongoAPI <options>" << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "\t-h    prints this help menu and exits" << std::endl;
    std::cout << "\t-t    is followed by name of method to be tested, then performs test and exits" << std::endl;
    std::cout << "\t-n    indicates that output should not be inserted into database" << std::endl;
    std::cout << "\t-s    indicates submodules are not to be tested" << std::endl;
    //std::cout << "\t--version prints ATL version information\n"<< std::endl;
    std::cout << "Method names to follow t are: MongoInterface" << std::endl;
    std::cout << std::endl;
    return 1;
}

/**
 * \brief main function
 **/
int main(int argc, char *argv[]) {

	bool testSubmodules = true;
	bool testAll = true;
	bool insert = true;

    //command line options
    int i;
    for(i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-t")) {
            if (testAll) {
                unitList.clear();
                testAll = false;
            }
            i++;
            unitList.push_back( argv[i]);
        } else if(strcmp(argv[i], "-n") == 0){
            //insert = false;
        } else if(strcmp(argv[i], "-s") == 0){
            testSubmodules = false;
        } else if(strcmp(argv[i], "-h") == 0){
            printHelp();
            return 1;
        } else {
            std::cout << "Bad option used..." << std::endl;
            std::cout << "Exiting!" << std::endl;
            return 1;
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
