/**
* Checking:
* creating/dropping collections functionality
*
**/

#include <string>
#include <thread>

#include "JsonBox.h"
#include "mongoapi.h"

using namespace std;

string uri = "127.0.0.1:27017";
uint16_t max_clients = 10;
string db_name = "test_acos";
int num_of_collections = 10;

mongoapi::MongoInterface mi(uri, max_clients);

struct Result {
   bool exists;
   bool removed; 
};

int main(int argc, char * argv[] )
{
  if (!mi.connect(db_name)) exit(1);
  
  vector<string> collections;
  for (int i = 0; i < num_of_collections; i++) {       
    JsonBox::Value value; 
    value["test"] = i;    
    collections.push_back("collection_" + to_string(i));

    mi.insert(collections[i], value);
  }

  map<string, Result> res;
  for (int i = 0; i < num_of_collections; i++) {
    JsonBox::Value value;
    value["test"] = i; 

    res[collections[i]].exists = (mi.query(collections[i], value).getArray().size() == 1);
  }

  for (int i = 0; i < num_of_collections; i++) {
    JsonBox::Value value;
    value["test"] = i;
    mi.dropCollection(collections[i]);
    
    res[collections[i]].removed = (mi.query(collections[i], value).getArray().size() == 0);
  }
      
   cout << "\t\t" << "exists\t" << "removed\t" << endl;
   for (map<string, Result>::iterator it = res.begin(); it != res.end(); it++) {
      cout << it->first << "\t"
           << (it->first.length() < 8 ? "\t" : "")
           << it->second.exists << "\t"       
           << it->second.removed << "\t"  
           << ((it->second.exists && it->second.removed) ? "pass" : "fail")
           << endl;
   }

   return 0;
}