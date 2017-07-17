/**
 * \file MongoAPITest.h
 *
 * Created on: June 19, 2017
 * Author: Nils Persson
 **/

#include "mongoapi.h"
#include "JsonBox.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include "JsonBox.h"
#include <vector>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/oid.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <assert.h>
#include <mongoapi/revision.h>
#include <mongoapi/revision.h>

namespace mongoapi {
/**
 * Perform unit tests for the mongoapi repository
 *
 * @param testSubmodules The boolean, if true run unit tests on submodules as well.
 * @return True on success
 */
JsonBox::Value testMongoAPI(std::vector<std::string> unitList = {"MongoInterface"}, bool testSubmodules = true);

/**
 * Perform unit tests for the MongoInterface class
 *
 * @param uri The string representation of the ip and port in format "IP:PORT"
 * @param printFlag The boolean, true if printed messages to console desired
 * @param assertFlag The boolean, true if quit desired upon error
 * @return True on success
 */
JsonBox::Value testMongoInterface(bool printFlag, bool assertFlag, std::string uri = "127.0.0.1:27017");
}