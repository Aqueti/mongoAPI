/**
 * \file MongoInterfaceTest.cpp
 *
 * Created on: June 19, 2017
 * Author: Nils Persson
 **/

#include "MongoAPITest.h"

namespace mongoapi {
JsonBox::Value testMongoInterface(std::string uri, bool printFlag, bool asserFlag){
	try {
		//create variables used in testing
		JsonBox::Value val1;
		JsonBox::Value val2;
		JsonBox::Value val3;
		JsonBox::Value val4;
		JsonBox::Value val5;
		JsonBox::Value val6;
		JsonBox::Value val7;
		JsonBox::Value val8;
		JsonBox::Value val9;
		JsonBox::Value val10;
		JsonBox::Value result;
		JsonBox::Value returnJson;
		int count;
		bool pass;
		returnJson["pass"] = true;

		//connect to database
		//default parameters are database: "test", URI: "127.0.0.1:27017"
                std::shared_ptr<MongoAPI> mi = getMongoAPI(uri);
//		MongoAPI mi(uri);
		bool connected = mi->connect("test");
		if(!connected){
			std::cout << "testing halted" << std::endl;
			returnJson["pass"] = false;
		}

		//test values for insert
		val1["name"] = "erik";
		val1["class"] = "senior";
		val1["age"] = 21;
		val2["name"] = "alex";
		val2["class"] = "sophomore";
		val2["age"] = 19;
		val3["name"] = "emma";
		val3["class"] = "freshman";
		val3["age"] = 14;
		val4["name"] = "emma";
		val4["class"] = "junior";
		val4["age"] = 14;

		//test value for query and remove
		val5["name"] = "emma";
		val8["name"] = "erik";
		val9["name"] = "alex";
		val10["$or"][(size_t)0] = val8;
		val10["$or"][1] = val9;

		//test value for update 
		val6["name"] = "erik";
		val7["$set"]["class"] = "graduate";

		if(connected){
			//drop collection before testing for a clean test environment
			mi->dropCollection("test");

			//test insert function of database (num and values)
			//will create test collection if it does not already exist
			mi->insert("test", val1);
			mi->insert("test", val2);
			mi->insert("test", val3);
			mi->insert("test", val4);

			count = mi->count("test");
			if(count != 4){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "incorrect number of values inserted: " + count << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			result = mi->queryAll("test");
			pass = true;
			if(result[(size_t)0]["name"] != val1["name"] || result[(size_t)0]["class"] != val1["class"]
					|| result[(size_t)0]["age"] != val1["age"]){
				pass = false;
			}
			if(result[1]["name"] != val2["name"] || result[1]["class"] != val2["class"]
					|| result[1]["age"] != val2["age"]){
				pass = false;
			}
			if(result[2]["name"] != val3["name"] || result[2]["class"] != val3["class"]
					|| result[2]["age"] != val3["age"]){
				pass = false;
			}
			if(result[3]["name"] != val4["name"] || result[3]["class"] != val4["class"]
					|| result[3]["age"] != val4["age"]){
				pass = false;
			}

			if(!pass){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "values incorrectly inserted" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			//test query function of database (num and values)
			result = mi->query("test", val5);
			count = result.getArray().size();
			if(count != 2){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "query attempt 1 failed; results expected: 2, received: " + count << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			pass = true;
			if(result[(size_t)0]["name"] != val3["name"] || result[(size_t)0]["class"] != val3["class"]
					|| result[(size_t)0]["age"] != val3["age"]){
				pass = false;
			}
			if(result[1]["name"] != val4["name"] || result[1]["class"] != val4["class"]
					|| result[1]["age"] != val4["age"]){
				pass = false;
			}

			if(!pass){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "query attempt 1 failed; incorrect values received" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			result = mi->query("test", val10);
			count = result.getArray().size();
			if(count != 2){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "query attempt 2 failed; results expected: 2, received: " + count << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			pass = true;
			if(result[(size_t)0]["name"] != val1["name"] || result[(size_t)0]["class"] != val1["class"]
					|| result[(size_t)0]["age"] != val1["age"]){
				pass = false;
			}
			if(result[1]["name"] != val2["name"] || result[1]["class"] != val2["class"]
					|| result[1]["age"] != val2["age"]){
				pass = false;
			}

			if(!pass){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "query attempt 2 failed; incorrect values received" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			//test remove function of database
			mi->removeEntry("test", val5);

			count = mi->countFilter("test", val5);
			if(count != 0){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "remove attempt failed, values matching filter still present" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}

			//test update function of database
			mi->update("test", val6, val7);

			result = mi->query("test", val6);
			if(result[(size_t)0]["class"] != "graduate"){
				returnJson["pass"] = false;
				if(printFlag){
					std::cout << "update attempt failed" << std::endl;
				}
				if(asserFlag){
					assert(false);
				}
			}
			
			//drop the collection to clean up the database after testing
			mi->dropCollection("test");
		}

		//return the Json value
		return returnJson;

	} catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
		JsonBox::Value returnJson;
		returnJson["pass"] = "false";
		return returnJson;
	} catch (const std::string& ex) {
		std::cout << ex << std::endl;
		JsonBox::Value returnJson;
		returnJson["pass"] = "false";
		return returnJson;
	} catch (...) {
		std::cout << "default exception";
		JsonBox::Value returnJson;
		returnJson["pass"] = "false";
		return returnJson;
	}
}	
}
