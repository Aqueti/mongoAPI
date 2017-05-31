/*
 * MongoInterface_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: cameron
 */

#include "MongoInterface.h"

int main() {
	try {
		//construct the interface object, defualt constructor creates connection to database
		//default parameters are database: "aqueti", ip_port: "127.0.0.1:27017"
		MongoInterface mi;
		JsonBox::Value val1;
		JsonBox::Value val2;
		JsonBox::Value val3;
		JsonBox::Value val4;
		JsonBox::Value val5;

		//test values for insert
		val1["name"] = "erik";
		val1["class"] = "senior";
		val2["name"] = "alex";
		val2["class"] = "sophomore";
		val3["name"] = "emma";
		val3["class"] = "freshman";
		val4["name"] = "emma";
		val4["class"] = "junior";

		//test value for query and remove
		val5["name"] = "emma";

		//test insert function of database
		//will create Test collection if it does not already exist
		mi.insertJSON("test", val1);
		mi.insertJSON("test", val2);
		mi.insertJSON("test", val3);
		mi.insertJSON("test", val4);

		//test query function of database
		std::cout << mi.query("test", val5) << std::endl;

		//test remove function of database
		mi.removeEntry("test", val5);

		//mi.insertJSON("Test", val1);
		//JsonBox::Value val2;
		/*
		val2["TestKey"]["$in"] = JsonBox::Array();
		val2["TestKey"]["$in"][size_t(0)] = JsonBox::Value("TestVal");
		val2["TestKey"]["$in"][size_t(1)] = JsonBox::Value("TestVal2");

		std::cout << val2 << std::endl;
		std::cout << mi.query("Test", val2) << std::endl;
		mi.removeEntry("Test", val1, false);
		std::cout << mi.query("test", val2) << std::endl;
		*/
	} catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return 1;
	} catch (const std::string& ex) {
		std::cout << ex << std::endl;
		return 1;
	} catch (...) {
		std::cout << "default exception";
		return 1;
	}
	return 0;
}
