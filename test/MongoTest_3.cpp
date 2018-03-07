/**
* Checking:
* update function
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

void run_task(map<string, vector<JsonBox::Value> > values, int task_id) { 
   JsonBox::Value filter, update; 
   filter["thread"] = task_id;
   update["$set"]["update"] = task_id;     
   for (map<string, vector<JsonBox::Value> >::iterator it = values.begin(); it != values.end(); it++) {
      for (int i = 0; i < it->second.size(); i++) {
         mi.update(it->first, filter, update);
      }
   }
}

struct Result {
   int exp;
   int act;
   bool cmp;
};

int main(int argc, char * argv[] )
{
   if (!mi.connect(db_name)) exit(1);

   uint16_t num_of_threads = 10;
   map<string, uint32_t> num_of;
   num_of["sessions"] = 10;
   num_of["files"] = 10;
   num_of["containers"] = 10;   

   if (isClean) for (map<string, uint32_t>::iterator it = num_of.begin(); it != num_of.end(); it++) mi.removeEntries(it->first, {});

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

   JsonBox::Value filter;
   filter["update"]["$gte"] = 0;
   filter["update"]["$lte"] = num_of_threads;
   res["sessions"].exp =  num_of_threads * num_of["sessions"];
   res["files"].exp =  num_of_threads * num_of["sessions"] * num_of["files"];
   res["containers"].exp =  num_of_threads * num_of["sessions"] * num_of["files"] * num_of["containers"];
   res["sessions"].act = mi.count("sessions", filter);
   res["files"].act = mi.count("files", filter);
   res["containers"].act = mi.count("containers", filter);
   
   for (int x = 0; x < num_of_threads; x++) {      
      for (map<string, uint32_t>::iterator it = num_of.begin(); it != num_of.end(); it++) {
         res[it->first].cmp = true;
         JsonBox::Value filter;
         filter["thread"] = x;       
         JsonBox::Value::Array v_arr = mi.query(it->first, filter).getArray();
         
         for (int i = 0; i < v_arr.size(); i++) {             
            JsonBox::Value cmp_val = values[x][it->first][i];            
            cmp_val["update"] = x;
            cmp_val["_id"] = v_arr[i]["_id"];

            if(res[it->first].cmp) res[it->first].cmp = (v_arr[i] == cmp_val);           
         }         
      }     
   }
      
   cout << "\t\t" << "exp\t" << "act\t" << "cmp\t" << endl;
   for (map<string, Result>::iterator it = res.begin(); it != res.end(); it++) {
      cout << it->first << "\t"
           << (it->first.length() < 8 ? "\t" : "")
           << it->second.exp << "\t"
           << it->second.act << "\t"
           << it->second.cmp << "\t"        
           << ((it->second.act == it->second.exp && it->second.cmp) ? "pass" : "fail")
           << endl;
   }

   return 0;
}
