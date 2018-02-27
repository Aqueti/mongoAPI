/**
 * @file mongoapi.h
 * @author Nils Persson <npersson@live.unc.edu>
 * @version 2.0
 *
 * Interface class to MongoDB
 *
 * A class to connect to a MongoDB database with methods to insert, retrieve,
 * and remove entries with JsonBox Values.
 */
#ifndef MONGOAPI_H_
#define MONGOAPI_H_

#include <JsonBox.h>
#include <vector>
#include <memory>

namespace mongoapi
{
/**
 * @class MongoAPI
 *
 * @brief A programming interface to a MongoDB database
 * with methods to insert, retrieve, and remove entries with JsonBox Values.
 */
class MongoAPI{
private:
   public:
      MongoAPI();
      virtual ~MongoAPI() {};
      virtual bool connect( std::string database ) {};
      virtual bool createIndex(std::string collection, JsonBox::Value index){};
      virtual std::string insert(std::string collection, JsonBox::Value data){};
      virtual int insertMany(std::string collection, std::vector<JsonBox::Value> data){};
      virtual std::string insertUnitTests(std::string collection, JsonBox::Value data){};
      virtual JsonBox::Value query(std::string collection, JsonBox::Value data){};
      virtual JsonBox::Value queryAll(std::string collection){};
      virtual bool removeEntry(std::string collection, JsonBox::Value data){};
      virtual bool update(std::string collection, JsonBox::Value filter, JsonBox::Value update){};
      virtual int count(std::string collection){};
      virtual int countFilter(std::string collection, JsonBox::Value filter){};
      virtual std::string getDatabase() const{};
      virtual std::string getURI() const{};
      virtual int getMaxClients() const{};
      virtual int getCreatedClients() const{};
      virtual bool removeAllEntries(std::string collection){};
      virtual bool dropCollection(std::string collection){};
};

   std::shared_ptr<MongoAPI> getMongoAPI( std::string uri );
}
#endif /* MONGOAPI_H_ */

