/**
* Checking:
* insert function
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
uint16_t num_of_threads = 10;
uint32_t times_per_thread = 1e3;
bool isClean = true;

mongoapi::MongoInterface mi(uri, max_clients);

void run_task(map<string, JsonBox::Value> value, int task_id) {   
   for (map<string, JsonBox::Value>::iterator it = value.begin(); it != value.end(); it++) {
      if (it->first != "containers") {
         mi.insert(it->first, value[it->first]);         
      } else {
         for (int i = 0; i < times_per_thread; i++) {
            value[it->first]["hcIndex"] = i;
            mi.insert(it->first, value[it->first]);
         }
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

   vector<string> collections = {"sessions", "files", "containers"};

   map<string, JsonBox::Value> values;
   values["sessions"]["active"] = "";
   values["sessions"]["directory"] = "";
   values["sessions"]["sessionID"] = "test";

   values["files"]["HContainerID"] = "1234567890";
   values["files"]["filePath"] = "";
   values["files"]["relativePath"] = "";
   values["files"]["sessionID"] = values["sessions"]["sessionID"].getToString();

   values["containers"]["HContainerID"] = values["files"]["HContainerID"].getToString();
   values["containers"]["containerType"] = "";
   values["containers"]["hcIndex"] = "";
   values["containers"]["mcamID"] = "";
   values["containers"]["sessionID"] = values["sessions"]["sessionID"].getToString();
   values["containers"]["tile"] = "";
   values["containers"]["tilingPolicy"] = "";
   values["containers"]["timestamp"] = "";

   vector<map<string, JsonBox::Value> > t_values;
   for (int i = 0; i < num_of_threads; i++) {
      t_values.push_back(values);
      t_values[i]["sessions"]["sessionID"] = t_values[i]["sessions"]["sessionID"].getToString() + to_string(i);
      t_values[i]["files"]["HContainerID"] = t_values[i]["files"]["HContainerID"].getToString() + to_string(i);    
      t_values[i]["files"]["sessionID"] = t_values[i]["sessions"]["sessionID"].getToString();
      t_values[i]["containers"]["sessionID"] = t_values[i]["sessions"]["sessionID"].getToString();
      t_values[i]["containers"]["HContainerID"] = t_values[i]["files"]["HContainerID"].getToString();
   }

   if (isClean) for (int i = 0; i < collections.size(); i++) mi.removeEntries(collections[i], {});

   vector<thread> threads;
   for (int i = 0; i < num_of_threads; i++) {
      threads.push_back(thread(run_task, t_values[i], i));
   }

   for (int i = 0; i < num_of_threads; i++) {
      threads[i].join();
   }

   map<string, Result> res;
   for (int i = 0; i < collections.size(); i++) {
      if (i != 2) res[collections[i]].exp = num_of_threads;
      else res[collections[i]].exp = num_of_threads * times_per_thread;

      res[collections[i]].act = mi.count(collections[i], {});
   }
      
   for (int i = 0; i < num_of_threads; i++) {
      for (map<string, JsonBox::Value>::iterator it = t_values[i].begin(); it != t_values[i].end(); it++) {
         JsonBox::Value condition;

         res[it->first].cmp = true;
         condition["sessionID"] = values[it->first]["sessionID"].getToString() + to_string(i);         
         
         if (it->first == "containers") {
            for (int j = 0; j < times_per_thread; j++) {          
               condition["hcIndex"] = j;                    
               condition["HContainerID"] = values[it->first]["HContainerID"].getToString() + to_string(i);
           
               if(res[it->first].cmp) res[it->first].cmp = (mi.query(it->first, condition).getArray().size() == 1);
            }
         } else {
            if (it->first == "files") condition["HContainerID"] = values[it->first]["HContainerID"].getToString() + to_string(i);
            
            if(res[it->first].cmp) res[it->first].cmp = (mi.query(it->first, condition).getArray().size() == 1);
         }
      }
   }
     
   cout << "\t\t" << "exp\t" << "act\t" << "cmp" << endl;
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
