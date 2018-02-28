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
      virtual bool connect( std::string database ) {return false;};
      virtual bool createIndex(std::string collection, JsonBox::Value index){return false;};
      virtual std::string insert(std::string collection, JsonBox::Value data){ return std::string("");};
      virtual int insertMany(std::string collection, std::vector<JsonBox::Value> data){return 0;};
      virtual std::string insertUnitTests(std::string collection, JsonBox::Value data){return std::string(); };
      virtual JsonBox::Value query(std::string collection, JsonBox::Value data){return JsonBox::Value();};
      virtual JsonBox::Value queryAll(std::string collection){return JsonBox::Value();};
      virtual bool removeEntry(std::string collection, JsonBox::Value data){return false;};
      virtual bool update(std::string collection, JsonBox::Value filter, JsonBox::Value update){return false;};
      virtual int count(std::string collection){return false;};
      virtual int countFilter(std::string collection, JsonBox::Value filter){return false;};
      virtual std::string getDatabase() const{ return std::string();};
      virtual std::string getURI() const{return std::string();};
      virtual int getMaxClients() const{return 0;};
      virtual int getCreatedClients() const{ return 0;};
      virtual bool removeAllEntries(std::string collection){return false;};
      virtual bool dropCollection(std::string collection){return false;};
};

   std::shared_ptr<MongoAPI> getMongoAPI( std::string uri );
}
#endif /* MONGOAPI_H_ */

