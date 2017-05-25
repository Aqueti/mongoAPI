/*
 * MongoInterface_test.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: cameron
 */

#include "MongoInterface.h"

int main() {
	try {
		MongoInterface mi;
		JsonBox::Value val1;
		val1["TestKey"] = "TestVal";
		mi.insertJSON("Test", &val1);
		JsonBox::Value val2;

		val2["TestKey"]["$in"] = JsonBox::Array();
		val2["TestKey"]["$in"][size_t(0)] = JsonBox::Value("TestVal");
		val2["TestKey"]["$in"][size_t(1)] = JsonBox::Value("TestVal2");

		std::cout << val2 << std::endl;
		std::cout << mi.query("Test", &val2) << std::endl;
		mi.removeEntry("Test", &val1, false);
		std::cout << mi.query("test", &val2) << std::endl;
	} catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return 1;
	} catch (const std::string& ex) {
		std::cout << ex << std::endl;
		return 1;
	} catch (...) {
		std::cout << "Something went wrong";
		return 1;
	}
	return 0;
}
