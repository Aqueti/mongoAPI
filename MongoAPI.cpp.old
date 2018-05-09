/**
 * @file MongoAPI.h
 * @author Steve Feller
 * @version 2.0
 *
 * Interface class to MongoDB
 *
 **/

#include "MongoAPI.h"
#include "MongoInterface.h"

namespace mongoapi
{
   /**
    * \brief Constructor.
    **/
   MongoAPI::MongoAPI()
   {
   }

   /**
    * \brief factory that creates a mongoDB instance and returns it as a MongoAPI reference
    **/
   std::shared_ptr<MongoAPI> getMongoAPI( std::string uri ) 
   {
      return getMongoDBInterface( uri );
   }
   
}
