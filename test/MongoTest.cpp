/**
 * \file MongoAPITest.cpp
 *
 * Created on: June 19, 2017
 * Author: Steve Feller/Nils Persson
 **/
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <JsonBox.h>
#include <cstring>


#include "MongoAPI.h"
#include <atl/TSQueue.tcc>
#include <atl/Timer.h>

//global variables
std::string uri = "127.0.0.1:27017";
std::string database = "test";
std::string collection = "test";

int verbose = 0;
double startTime;
bool done = false;
uint16_t statusRate = 1;  //Rate to print status in seconds
bool useGlobalDatabase = true;

uint64_t writeThreads = 10;
uint64_t writeSuccess = 0;
uint64_t writeFail = 0;
uint64_t writePS= 4000;
double   writeRate= 0;
double   lastWriteTime = 0;                     //Time of last write to prevent queries into the future
uint64_t insertCount = 0;


bool     windowMode = true;
uint64_t queryThreads = 1;
uint64_t querySuccess = 0;
uint64_t queryFail = 0;
uint64_t queryPS= 450;
double   queryRate = 0.0;
double   queryDelay = 10.0;

//Thread safe queues for passing data between threads
atl::TSQueue<JsonBox::Value> inputQ;     //We are passing in a JsonBox metadata and an destination
atl::TSQueue<JsonBox::Value> queryQ;                              //Queue of JsonBox queries 
atl::TSQueue<std::tuple<JsonBox::Value, JsonBox::Value>> outputQ;     //Found Result, destination, query 

//Mongo interface object
std::shared_ptr<mongoapi::MongoAPI> mongoAPI;


/**
 * \brief Callback for a successful query. 
 * 
 * \param [in] requestId tuple that contains <threadId, requestId> of the query generation thread
 **/
//void queryCallback( std::tuple<uint64_t, uint64_t>, JsonBox::Value result, JsonBox::Value query );


/*
uint64_t writeCount = 0;
double writeTime = 0;

double writeStartTime = 0;
double queryStart = 0;
double queryDelay = 1.0;
*/

/*
std::vector<std::tuple<int64_t,double>> writeThreadTime;  //query in time. 
std::vector<std::tuple<int64_t,double>> queryThreadTime;  //tracks queries per thread 

atl::Timer insertTimer;
atl::Timer queryTimer;
*/

/**
 * \generates a header (for requests or queries) based on ID
 *
 * \param in id sequence number all other information is generated from 
 **/
JsonBox::Value metaGenerator( int id ) {
   std::stringstream ss;
   ss<<"file_"<<id;

   double ts = (double)startTime+(double)id * writeRate;

   JsonBox::Value input;
   input["id"].setString("test");
   input["timestamp"].setString(std::to_string(ts));
   input["name"] = ss.str();

   return input;
}

/**
 * \brief function to print controls
 **/
void printControls()
{
   std::cout << "\n> Control Options:\n> Enter one of the following commands to modify execution"<<std::endl;
   std::cout << ">\texit - stop execution"<<std::endl;
   std::cout << ">\thelp - print this help menu"<<std::endl;
}


/** 
 * \brief Keyboard control thread
 **/
void ControlThread() {
   std::string input;
   printControls();

   while( !done ) {
       std::cout << "> ";

       std::cin >> input;

       if( !input.compare("exit")) {
           done = true;
       }
       else if( !input.compare("help")) {
           printControls();
       }
       else {
           std::cout << "> Unrecognized command. Enter help for options"<<std::endl;
       }
   }
}

/**
 * \thread to print out information about status
 **/
void StatusThread() {
   uint64_t lastWriteSuccess = writeSuccess;
   uint64_t lastWriteCount   = writeSuccess+writeFail;
   uint64_t lastQuerySuccess = querySuccess;
   uint64_t lastQueryCount   = querySuccess+queryFail;
//   uint64_t lastInsert = insertCount;

   double writePercent = 0.0;
   double queryPercent = 0.0;

   while( !done) {
      uint64_t totalWrites = writeSuccess + writeFail;
      uint64_t totalQueries = querySuccess + queryFail;

      uint64_t totalWritesPerIter  = totalWrites  - lastWriteCount;
      uint64_t writeSuccessPerIter = writeSuccess - lastWriteSuccess;
      uint64_t totalQueriesPerIter = totalQueries - lastQueryCount;
      uint64_t querySuccessPerIter = querySuccess - lastQuerySuccess;

      if(totalWritesPerIter > 0 ) {
         writePercent = 100.0*(double)writeSuccessPerIter/(double)totalWritesPerIter;
      }
      if(( totalQueriesPerIter ) > 0 ) {
         queryPercent = 100.0*(double)querySuccessPerIter/(double)totalQueriesPerIter;
      }
 
      std::stringstream ss;

      std::cout << atl::getDateAsString()
                << "\tWrites:"<<writeSuccessPerIter<<"/"<<totalWritesPerIter<<"("<<writePercent<<"%," << writeSuccessPerIter << " wps)"
                << "\tQueries:"<<querySuccessPerIter<<"/"<<totalQueriesPerIter<<"("<<queryPercent<<"%)," << querySuccessPerIter << " qps)"
                << "\tinputQ: "<<inputQ.size()<<", queryQ: "<<queryQ.size()
                <<std::endl;

      lastWriteCount = lastWriteCount + totalWritesPerIter;
      lastWriteSuccess = writeSuccess;
      lastQueryCount =   lastQueryCount + totalQueriesPerIter;
      lastQuerySuccess = querySuccess;
//      lastInsert = insertCount;

      std::this_thread::sleep_for(std::chrono::microseconds((uint64_t)(statusRate *(double)1e6)));
//      atl::sleep(statusRate);
   }
}

/**
 * \brief Thread to write multiple records to a databasea
 * \param [in] tid thread id
 * \param [in] doneFlag pointer to a boolean to stop operation
 * \param [in] tid number of the thread
 * \param [in] collection name of the collection to use
 *
 * This function writes count records to a database. The first record calculated
 * as the thread id * the number of records per 
 **/
void WriteThread( unsigned int tid, bool * doneFlag  ) {
   std::shared_ptr<mongoapi::MongoAPI> localInterface = mongoapi::getMongoAPI(uri);
   if( !useGlobalDatabase ) {
      //Try to connect to the database
      bool connected = localInterface->connect(database);
      if(!connected){
         std::cout << "writeThread "<<std::fixed<<tid<<" unable to connect to the database. Exiting" << std::endl;
         exit(1);
      }
   }

   //This is what we'll be dequeuing. First tuple is header, second is destination directory
   JsonBox::Value input; 

   //Loop until done
   while( !*doneFlag ) {

      //Pull the next item from the Queue
      inputQ.dequeue( input,1000);
      if( verbose > 1 ) {
          std::cout << "Thread "<<tid<<" received value. Writing "<<input<<" to "<<collection<<std::endl;
      }

      //If the input is null, nothing was available
      if( !input.isNull()) {
         //Add item to the database
         std::string out;

         if( !useGlobalDatabase ) {
            out = localInterface->insert(collection, input);
         }
         else {
            out = mongoAPI->insert(collection, input);
         }
         if( out.empty()) {
             std::cout<< tid <<" - Write failed!"<<std::endl;
             writeFail++;
         }
         else {
            if( std::stod(input["timestamp"].getString()) > lastWriteTime ) {
               lastWriteTime = std::stod(input["timestamp"].getString());
            }
            writeSuccess++;
         }

      }
   }

   std::cout<< "Write thread "<<tid<<" exiting!"<<std::endl;
}

/**
 * \brief Thread to query multiple records to a databasea
 * \param [in] timestamp to start looking for
 * \param [in] count number of items to find
 *
 * This function reads count records to a database. The first record calculated
 * as the thread id * the number of records per 
 **/
void QueryThread( unsigned int tid, std::string collection, bool *doneFlag  ) {

   std::shared_ptr<mongoapi::MongoAPI> localInterface = mongoapi::getMongoAPI(uri);
   
   if( !useGlobalDatabase ) {
      //Try to connect to the database
      bool connected = localInterface->connect(database);
      if(!connected){
         std::cout << "queryThread "<<tid<<" unable to connect to the database. Exiting" << std::endl;
         return;
      }
   }
   

   //Loop until done
   while( !*doneFlag ) {
      JsonBox::Value request;

      //Pull the next item from the Queue
      queryQ.dequeue( request,1000);

      if( request.isNull()) {
         if(verbose > 1) {
            std::cout << tid <<" Query Thread did not receive any data"<<std::endl;
         }
      }
     //Actual query step
      else {
         if( verbose > 1) {
            std::cout << "Query Thread "<<tid<<" received value of "<<request<<std::endl;
         }
         JsonBox::Value query;  //Query representation

         //Define a window for queries. This is for the case where we do not know exact time
         if( windowMode ) {
 
            double windowStart =std::stod(request["timestamp"].getString());
            windowStart = windowStart - writeRate/2.0;

            JsonBox::Value timebounds;
            timebounds["$gte"].setString(std::to_string(windowStart));
            timebounds["$lt"].setString(std::to_string(windowStart+writeRate));

            query["id"] = request["id"];
            query["timestamp"] = timebounds;
         }
         //Direct index query
         else {
            query["id"] = request["id"];
            query["timestamp"] = request["timestamp"].getString();
         }
    
         //Perform actual query
         JsonBox::Value result;
         if( !useGlobalDatabase ) {
            result = localInterface->query(collection, query);
         }
         else {
            result = mongoAPI->query(collection, query);
         }

         //If null, query failed!
         if( result.isNull()) {
            std::cout << tid << " DB error - Failed with a null return query: "<<query<<std::endl;
            queryFail++;
         }

         else { 
            JsonBox::Array arr = result.getArray();

            //If the array has zero entries, that means nothing was found. 
            if( arr.size() == 0 ) {
               std::cout << tid << " No result found!  "<<query<<std::endl;
               queryFail++;
            }
            else {
               double target = std::stod(arr[0]["timestamp"].getString());
               double actual=  std::stod(arr[0]["timestamp"].getString());

//               double diff = std::abs(actual-target);
            
               //Make sure the data we recieved is within the window
               if((std::abs( target-actual ) > writeRate/2.0)) {
                  std::cout << tid <<" query timestamp error. Difference in target,actual exceeds rate. ("
                            <<target<<","<<actual<<" > "<<writeRate/2.0<<")"<<std::endl;
               } 
               else {
                  if( arr.size() > 1 ) { 
                     if(verbose) {
                        std::cout << "Received "<<arr.size()<<" records when expecting 1 from query: "<<query<<std::endl;
                     }
                  }
                  else {
                     querySuccess++;
//                  outputQ.enqueue(std::tuple<JsonBox::Value,JsonBox::Value>(arr[0], request));
                  }
               }
            }
         }
      }
   }
}

/**
 * \brief This thread handles query requests
 **/
void QueryController() {
   bool queryDone = false;
   std::vector<std::thread> threadVect;


   //Spawn query threads that each write m values
   for( uint64_t i = 0; i < queryThreads; i ++ ) {
      threadVect.push_back( std::thread( QueryThread, i, collection, &queryDone));
   }

   std::cout << "In query controller. Waiting for "<<std::fixed<<queryDelay<<" seconds to begin queries"<<std::endl;
   atl::sleep(queryDelay);
   std::cout << "In query controller. Sending queries at about "<<queryPS<<" fps"<<std::endl;

   //Add data to the query queues
   uint64_t i = 1;
   while( !done ) {
      JsonBox::Value input = metaGenerator(i);

      double ts = std::stod(input["timestamp"].getString());
      while(( ts  > lastWriteTime )&&(!done)) {
         std::cout << "Query Controller Caught up("<<std::fixed<<ts-lastWriteTime<<"). Sleeping for 1 second"<<std::endl;
         atl::sleep(1);
         std::cout << "Query Controller awake again"<<std::endl;
      }

      queryQ.enqueue(input);
      i++;
      std::this_thread::sleep_for(std::chrono::microseconds((uint64_t)(queryRate *(double)1e6)));
//      atl::sleep(queryRate);
   }

   queryDone = true;

   //Wait for all threads to complete
   for( auto it = threadVect.begin(); it != threadVect.end(); it++ ) {
      it->join();
   }

   std::cout << "QueryController exiting"<<std::endl;
}


/**
 * \brief Prints the command line help
 **/
void printHelp()
{
   std::cout << "Database testing application\n"<<std::endl;
   std::cout << "option:"<<std::endl;
   std::cout << "\t-h            print this help menu"<<std::endl;
   std::cout << "\t-erase        erase test collection before testing"<<std::endl;
   std::cout << "\t-localdb      create a separate database interface for each thread"<<std::endl;
   std::cout << "\t-qd  <double> how long to delay queries after write starts (default="<<queryDelay<<")"<<std::endl;
   std::cout << "\t-qps  <double> target number of queries per second. (default="<<queryPS<<")"<<std::endl;
   std::cout << "\t-qt  <int>    number of query threads (default="<<queryThreads<<")"<<std::endl;
   std::cout << "\t-uri <string> URI of the database in the format of hostname:port. (default="<<uri<<")"<<std::endl;
   std::cout << "\t-v   <int>    enter verbose mode (default="<<verbose<<")"<<std::endl;
   std::cout << "\t-wt   <int>   number of write threads to use (default="<<writeThreads<<")"<<std::endl;
   std::cout << "\t-wps  <double> target number of writes per second. (default="<<writePS<<")"<<std::endl;
/*
   std::cout << "\t-rt  <int>   number of read threads (default="<<readThreads<<")"<<std::endl;
   std::cout << "\t-rpt <int>   reads per thread (default="<<readsPerThread<<")"<<std::endl
   std::cout << "\t-rr  <float> read rate where 0.0 = max (default="<<readRate<<")"<<std::endl;
*/

   std::cout <<"\n"<<std::endl;
}

/**
 * \brief Main function
 **/
int main(int argc, char * argv[] ) 
{

   bool erase = false;

   //Parse command line options
   int i = 1;
   while( i < argc ) {
      if( !strcmp( argv[i], "-erase")) {
         std::cout << "Erasing!"<<std::endl;
         erase = true;
      }
      else if( !strcmp( argv[i], "-h")) {
         printHelp();
         exit(1);
      }
      else if( !strcmp( argv[i], "-localdb")) {
         useGlobalDatabase = false;
      }
      else if( !strcmp( argv[i], "-qps")) {
         i++;
         queryPS = std::stod(argv[i]);
      }
      else if( !strcmp( argv[i], "-qt")) {
    i++;
    queryThreads = std::stoi(argv[i]);
      }
      else if( !strcmp( argv[i], "-uri")) {
    i++;
    uri = std::string(argv[i]);
      }
      else if( !strcmp( argv[i], "-wt")) {
    i++;
    writeThreads = std::stoi(argv[i]);
      }
      else if( !strcmp( argv[i], "-wps")) {
         i++;
         writePS = std::stod(argv[i]);
     }
     else if( !strcmp( argv[i], "-v")) {
        i++;
        verbose = std::stoi(argv[i]);
     }
     else {
        std::cout <<"Invalid command: "<<argv[i]<<". Run with the -h to see options\n\n"<<std::endl;                
        exit(1);
     }

     i++;
   }

   std::cout << "Interface to database at "<<uri<<" with the following configuration:\n"
             << "\t- Write Threads:     "<<writeThreads<<"\n"
             << "\t- Query Threads:     "<<queryThreads<<"\n"
             << "\t- Target Write(fps): "<<writePS<<"\n"
             << "\t- Target Query(fps): "<<queryPS<<"\n"
             << "\t- Query Delay:       "<<queryDelay<<"\n"
             << "\t- Query Delay:       "<<queryDelay<<"\n"
             << "\t- Verbose Mode:      "<<verbose<<"\n"
             << std::endl;
             




   startTime = atl::getTime();

   //Try to connect to the database
   mongoAPI = mongoapi::getMongoAPI(uri);
   bool connected = mongoAPI->connect(database);
   if(!connected){
      std::cout << "Unable to connect to the database to erase the "<<collection<<" collection. Exiting" << std::endl;
      exit(1);
   }

   //If the erase flag is set, remove all records
   if( erase ) {
      std::cout << "Removing all entries in the collection "<<collection<<". This may take a while."<<std::endl;
      mongoAPI->removeAllEntries(collection);
      std::cout << "All entries in the collection "<<collection<<" removed"<<std::endl;

      //Create the collection through the first entry
      JsonBox::Value first = metaGenerator(0);

      //Add item to the database
      JsonBox::Value input = metaGenerator(0);
      std::string out = mongoAPI->insert(collection, input);
      if( out.empty()) {
         writeFail++;
      }
      else if( std::stod(input["timestamp"].getString()) > lastWriteTime ) {
         lastWriteTime = std::stod(input["timestamp"].getString());
      }

      //Creat the collection index
      JsonBox::Value index1;
      index1["timestamp"].setInteger(1);
      if( verbose ) {
         std::cout << "Creating index "<<index1<<" for collection "<<collection<<std::endl;
      }
      mongoAPI->createIndex( collection, index1);
   }

   //Kickoff management threads
   std::thread controlThread = std::thread(ControlThread);
   std::thread status;        //Thread the checks the status of a running process

   if( queryThreads == 0 ) { 
      std::cout << "No queries will be made" << std::endl;
   }
   writeRate = 1.0/(double)writePS;
   queryRate = 1.0/(double)queryPS;

   //Create the status thread if we are in verbose mode
   if( verbose ) {
      status = std::thread(StatusThread);
   }  
   std::vector<std::thread> threadVect;

   //Spawn write threads that each write m values
   for( uint64_t i = 0; i < writeThreads; i ++ ) {
      threadVect.push_back( std::thread( WriteThread, i, &done));
   }


   //Spawn query controller. this will create and manage query threads
   std::thread queryController = (std::thread(QueryController));

   //Use the to create artificial metadata
   insertCount = 1;
   std::cout << "Inserting at "<<std::fixed<<writeRate<<" or "<<writePS<<" wps"<<std::endl;
   while( !done ) {
      JsonBox::Value input = metaGenerator(insertCount);
      inputQ.enqueue( input );
      insertCount++;

      std::this_thread::sleep_for(std::chrono::microseconds((uint64_t)(writeRate *(double)1e6)));
//      atl::sleep(writeRate);
   }

   std::cout << "No longer writing data. Preparing to exit"<<std::endl;
   done = true;

   queryController.join();
   //Wait for all threads to complete
   for( auto it = threadVect.begin(); it != threadVect.end(); it++ ) {
      it->join();
   }



   //Join the status thread
   if( verbose ) {
      status.join();
   }
   controlThread.join();

}
