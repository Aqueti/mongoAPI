/**
* Checking:
* remove function
*
**/

#include <string>
#include <thread>
#include <cmath>

#include "JsonBox.h"
#include "mongoapi.h"

using namespace std;

string uri = "127.0.0.1:27017";
uint16_t max_clients = 10;
string db_name = "test_acos";
uint16_t num_of_threads = 9;
uint16_t p_num_of_threads = 4;
bool isClean = true;

mongoapi::MongoInterface mi(uri, max_clients);

void run_task(map<string, vector<JsonBox::Value> > values, int task_id) { 
   JsonBox::Value value;
   value["thread"]["$gte"] = 0;
   value["thread"]["$lte"] = p_num_of_threads;

   for (map<string, vector<JsonBox::Value> >::iterator it = values.begin(); it != values.end(); it++) {
      mi.removeEntry(it->first, value);
   }
}

struct Result {
   int exp;
   int act; 
};

int main(int argc, char * argv[] )
{
   if (!mi.connect(db_name)) exit(1);
   
   map<string, uint32_t> num_of;
   num_of["sessions"] = 10;
   num_of["files"] = 10;
   num_of["containers"] = 10;

   if (isClean) for (map<string, uint32_t>::iterator it = num_of.begin(); it != num_of.end(); it++) mi.removeAllEntries(it->first);

   map<string, vector<JsonBox::Value> > v;
   vector<map<string, vector<JsonBox::Value> > > values;
   for (int x = 0; x < num_of_threads; x++) {
     values.push_back(v);

     for (int i = 0; i < num_of["sessions"]; i++) {
       JsonBox::Value value;
       value["active"] = "";
       value["directory"] = "";
       value["sessionID"] = "test" + to_string(i);    
       value["thread"] = x;
       value["update"] = -1;
       values[x]["sessions"].push_back(value);   

       for (int j = 0; j < num_of["files"]; j++) {      
         JsonBox::Value value;
         value["HContainerID"] = "0000000_" + to_string(i) + to_string(j);
         value["filePath"] = "";
         value["relativePath"] = "";
         value["sessionID"] = values[x]["sessions"][i]["sessionID"].getToString();
         value["thread"] = x;
         value["update"] = -1;
         values[x]["files"].push_back(value);      

         for (int k = 0; k < num_of["containers"]; k++) {
           JsonBox::Value value;
           value["HContainerID"] = values[x]["files"][j]["HContainerID"].getToString();
           value["containerType"] = "";
           value["hcIndex"] = k;
           value["mcamID"] = "";
           value["sessionID"] = values[x]["sessions"][i]["sessionID"].getToString();
           value["tile"] = "";
           value["tilingPolicy"] = "";
           value["timestamp"] = "";
           value["thread"] = x;
           value["update"] = -1;
           values[x]["containers"].push_back(value);
          }
        }
      }
    }

   for (int i = 0; i < values.size(); i++) {
      for (map<string, vector<JsonBox::Value> >::iterator it = values[i].begin(); it != values[i].end(); it++) { 
         mi.insertMany(it->first, it->second);
      }
   }

   vector<thread> threads;
   for (int i = 0; i < num_of_threads; i++) {
      threads.push_back(thread(run_task, values[i], i));
   }

   for (int i = 0; i < num_of_threads; i++) {
      threads[i].join();
   }

   map<string, Result> res;

   res["sessions"].exp =  p_num_of_threads * num_of["sessions"];
   res["files"].exp =  p_num_of_threads * num_of["sessions"] * num_of["files"];
   res["containers"].exp =  p_num_of_threads * num_of["sessions"] * num_of["files"] * num_of["containers"];
   res["sessions"].act = mi.count("sessions");
   res["files"].act = mi.count("files");
   res["containers"].act = mi.count("containers");
      
   cout << "\t\t" << "exp\t" << "act\t" << endl;
   for (map<string, Result>::iterator it = res.begin(); it != res.end(); it++) {
      cout << it->first << "\t"
           << (it->first.length() < 8 ? "\t" : "")
           << it->second.exp << "\t"
           << it->second.act << "\t"             
           << ((it->second.act == it->second.exp) ? "pass" : "fail")
           << endl;
   }

   return 0;
}