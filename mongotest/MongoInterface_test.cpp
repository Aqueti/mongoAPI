/*
 * MongoInterface_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: cameron
 */

#include "MongoInterface.h"
using namespace mongoAPI;

int main() {

	// initializes a connection
    MongoInterface interface( "test", "10.0.0.160", 27017 );

    // gets database information
    JsonBox::Value dbinfo = interface.getDBInfo();

    // JsonBox::Value query = interface.query("test","restaurant");


	// try {

    // inserts a value
	JsonBox::Value val1 = new JsonBox::Value();
	(val1)["TestKey"] = JsonBox::Value("TestVal");
	std::cout << interface.insertJSON("test", val1) << std::endl;


	JsonBox::Value val2 = new JsonBox::Value();
	(val2)["TestKey"]["$in"][size_t(0)] = JsonBox::Value("TestVal");
	(val2)["TestKey"]["$in"][1] = JsonBox::Value("TestVal2");

	// prints val1
	// std::cout << val1 << std::endl;

	// query val1
	std::cout << interface.query("test", val1) << std::endl;

	// std::cout << interface.query("restaurants", val1) << std::endl;

	// 	mi->removeEntry("Test", val1, false);
	// 	std::cout << mi->query("test", val2) << std::endl;
	// 	delete val2;
	// 	delete val1;
	// 	delete mi;
	// } catch (const std::exception& ex) {
	// 	std::cout << ex.what() << std::endl;
	// 	return 1;
	// } catch (const std::string& ex) {
	// 	std::cout << ex << std::endl;
	// 	return 1;
	// } catch (...) {
	// 	std::cout << "Something went wrong";
	// 	return 1;
	// }
	// return 0;
}
