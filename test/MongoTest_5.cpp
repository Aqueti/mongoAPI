/**
* Checking:
* getters functions
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
bool isClean = true;

mongoapi::MongoInterface mi(uri, max_clients);

struct Result {
   string exp;
   string act; 
};

int main(int argc, char * argv[] )
{
   if (!mi.connect(db_name)) exit(1);
  
   map<string, Result> res;

   res["database"].exp = db_name;
   res["uri"].exp = uri;
   res["max_clients"].exp = to_string(max_clients);
   res["cr_clients"].exp = to_string(max_clients);

   res["database"].act = mi.getDatabase();
   res["uri"].act = mi.getURI();
   res["max_clients"].act = to_string(mi.getMaxClients());
   res["cr_clients"].act = to_string(mi.getCreatedClients());
      
   cout << "\t\t" << "exp\t\t" << "act\t" << endl;
   for (map<string, Result>::iterator it = res.begin(); it != res.end(); it++) {
      cout << it->first << "\t"
           << (it->first.length() < 8 ? "\t" : "")
           << it->second.exp << "\t"
           << (it->second.exp.length() < 8 ? "\t" : "")
           << it->second.act << "\t"
           << (it->second.act.length() < 8 ? "\t" : "")    
           << ((it->second.act == it->second.exp) ? "pass" : "fail")
           << endl;
   }

   return 0;
}