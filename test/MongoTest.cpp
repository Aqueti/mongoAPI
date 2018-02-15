/**
 * \file MongoAPITest.cpp
 *
 * Created on: June 19, 2017
 * Author: Steve Feller/Nils Persson
 **/
#include <vector>
#include <thread>
#include <JsonBox.h>

#include "mongoapi.h"
#include <Timer.h>


//global variables
std::string uri = "127.0.0.1:27017";
std::string database = "test";
std::string collection = "test";
int writeThreads = 10;
int totalWrites = 100000;
int totalQueries = 100000;
double writeRate = 0.00001;
int queryThreads = 1;
int verbose = 0;
uint64_t querySuccess = 0;
uint64_t queryFail = 0;
uint64_t writeCount = 0;
uint64_t writeFail = 0;
double writeTime = 0;

double writeStartTime = 0;
double queryStart = 0;
double queryDelay = 1.0;
double queryRate = 0.00001;
bool done = false;
double lastWriteTime = 0;

std::vector<std::tuple<int64_t,double>> writeThreadTime;  //query in time. 
std::vector<std::tuple<int64_t,double>> queryThreadTime;  //tracks queries per thread 

atl::Timer insertTimer;
atl::Timer queryTimer;

 //Thread safe queues for passing data between threads
atl::TSQueue<JsonBox::Value> inputQ;     //We are passing in a JsonBox metadata and an destination
atl::TSQueue<JsonBox::Value> queryQ;                              //Queue of JsonBox queries 
atl::TSQueue<std::tuple<JsonBox::Value, JsonBox::Value>> outputQ;     //Found Result, destination, query 

/**
 * \generates a header (for requests or queries) based on ID
 *
 * \param in id sequence number all other information is generated from 
 **/
JsonBox::Value metaGenerator( int id ) {
   std::stringstream ss;
   ss<<"file_"<<id;

   double ts = (double)writeStartTime+(double)id*writeRate;

   JsonBox::Value input;
   input["id"].setString("test");
   input["timestamp"].setString(std::to_string(ts));
   input["name"] = ss.str();

   return input;
}


/**
 * \thread to print out information about status
 **/
void StatusThread() {
   uint64_t lastWriteCount = writeCount;
   uint64_t lastQueryCount = querySuccess;

   double writePercent = 0.0;
   double queryPercent = 0.0;

   while( !done) {
      if(( writeCount + writeFail ) > 0 ) {
         writePercent = 100.0*(double)writeCount/(double)(writeCount+writeFail);
      }
      if(( querySuccess + queryFail ) > 0 ) {
         queryPercent = 100.0*(double)(querySuccess)/(double)(querySuccess+queryFail);
      }
 
      std::cout << atl::getDateAsString()
                << ":    \tWrites:"<<writeCount<<"/"<<writeCount+writeFail<<"<<("<<writePercent<<"%) at " << writeCount - lastWriteCount<< " per second"
                << "     \tQueries:"<<querySuccess<<"/"<<querySuccess+queryFail<<"("<<queryPercent<<"%) at " << querySuccess - lastQueryCount << " per second" <<std::endl;
      lastWriteCount = writeCount;
      lastQueryCount = querySuccess;
      atl::sleep(1);
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

   //Try to connect to the database
   mongoapi::MongoInterface mongoInterface(uri);
   bool connected = mongoInterface.connect(database);
   if(!connected){
      std::cout << "writeThread "<<std::fixed<<tid<<" unable to connect to the database. Exiting" << std::endl;
      exit(1);
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

      //Add item to the database
      std::string out = mongoInterface.insert(collection, input);
      if( std::stod(input["timestamp"].getString()) > lastWriteTime ) {
         lastWriteTime = std::stod(input["timestamp"].getString());
      }

      writeCount++;
   }
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

   //Try to connect to the database
   mongoapi::MongoInterface mongoInterface(uri);
   bool connected = mongoInterface.connect(database);
   if(!connected){
      std::cout << "queryThread "<<tid<<" unable to connect to the database. Exiting" << std::endl;
      return;
   }
   
   //This is what we'll be dequeuing. First tuple is header, second is destination directory
   JsonBox::Value request;

   //Loop until done
   while( !*doneFlag ) {
      //Pull the next item from the Queue
      queryQ.dequeue( request,1000);
      if( verbose > 1) {
          std::cout << "Query Thread "<<tid<<" received value of "<<request<<std::endl;
      }

      if( request.isNull()) {
         if(verbose > 1) {
            std::cout << tid <<" Query Thread did not receive any data"<<std::endl;
         }
      }
      else {
         double windowStart =std::stod(request["timestamp"].getString())-writeRate/2.0;

         JsonBox::Value timebounds;
         timebounds["$gte"].setString(std::to_string(windowStart));
         timebounds["$lt"].setString(std::to_string(windowStart+writeRate));

         JsonBox::Value query;
         query["id"] = request["id"];
         query["timestamp"] = timebounds;
    
         JsonBox::Value result = mongoInterface.query(collection, query);

         if( result.isNull()) {
            std::cout << tid << " Failed with a null return query: "<<query<<std::endl;
         }

         else { 
            JsonBox::Array arr = result.getArray();

            if( arr.size() == 0 ) {
               std::cout << tid << " Failed with an empty array with query: "<<query<<std::endl;
               queryFail++;
            }

            else {
               if( arr.size() > 1 ) { 
                  if(verbose) {
                     std::cout << "Received "<<arr.size()<<" records when expecting 1 from query: "<<query<<std::endl;
                  }
               }

               querySuccess++;
               outputQ.enqueue(std::tuple<JsonBox::Value,JsonBox::Value>(result, request));
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
   for( int i = 0; i < queryThreads; i ++ ) {
      threadVect.push_back( std::thread( QueryThread, i, collection, &queryDone));
   }

   std::cout << "In query controller. Waiting for 10 to begin queries"<<std::endl;
   atl::sleep(10);

   //Add data to the query queues
   for( int i = 0; i < totalQueries, !done; i++ ) {
      JsonBox::Value input = metaGenerator(i);

      double ts = std::stod(input["timestamp"].getString());
      while(( ts  > lastWriteTime )&&(!done)) {
         std::cout << "Query Controller Caught up. Sleeping for 1 second"<<std::endl;
//         atl::sleep(queryRate);
         atl::sleep(1);
         std::cout << "Query Controller awake again"<<std::endl;
      }

      queryQ.enqueue(input);
      atl::sleep(queryRate);
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
   std::cout << "\t-qr  <double> query rate where 0.0 = max (default="<<queryRate<<")"<<std::endl;
   std::cout << "\t-qt  <int>    number of query threads (default="<<queryThreads<<")"<<std::endl;
   std::cout << "\t-uri <string> URI of the database in the format of hostname:port. (default="<<uri<<")"<<std::endl;
   std::cout << "\t-v   <int>    enter verbose mode (default="<<verbose<<")"<<std::endl;
   std::cout << "\t-w   <int>    total number of writes (default="<<totalWrites<<")"<<std::endl;
   std::cout << "\t-wt   <int>   number of write threads to use (default="<<writeThreads<<")"<<std::endl;
   std::cout << "\t-wr  <double> write rate per thread where 0.0 = max (default="<<writeRate<<")"<<std::endl;
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
   std::thread status;        //Thread the checks the status of a running process

   bool erase = false;
   int queriesPerThread = 0;
   queryTimer.stop();
   queryTimer.reset();
   insertTimer.stop();
   insertTimer.reset();



   //Parse command line options
   int i = 1;
   while( i < argc ) {
      if( !strcmp( argv[i], "-erase")) {
    erase = true;
      }
      else if( !strcmp( argv[i], "-h")) {
    printHelp();
    exit(1);
      }
      else if( !strcmp( argv[i], "-qr")) {
    i++;
    queryRate = std::stod(argv[i]);
      }
      else if( !strcmp( argv[i], "-qt")) {
    i++;
    queryThreads = std::stoi(argv[i]);
      }
      else if( !strcmp( argv[i], "-uri")) {
    i++;
    uri = std::string(argv[i]);
      }
      else if( !strcmp( argv[i], "-w")) {
    i++;
    totalWrites = std::stoi(argv[i]);
      }
      else if( !strcmp( argv[i], "-wt")) {
    i++;
    writeThreads = std::stoi(argv[i]);
      }
      else if( !strcmp( argv[i], "-wr")) {
    i++;
    writeRate = std::stod(argv[i]);
      }
      else if( !strcmp( argv[i], "-v")) {
              i++;
    verbose = std::stoi(argv[i]);
      }
      else {
    std::cout <<"Invalid command: "<<argv[i]<<". Run with the -h to see options\n\n"<<std::endl;
    
      }

      i++;
   }

   if( queryThreads == 0 ) { 
      std::cout << "No queries will be made" << std::endl;
   }

   //If the erase flag is set, remove all records
   if( erase ) {
      //Try to connect to the database
      mongoapi::MongoInterface mongoInterface(uri);
      bool connected = mongoInterface.connect(database);
      if(!connected){
    std::cout << "Unable to connect to the database to erase the "<<collection<<" collection. Exiting" << std::endl;
    exit(1);
      }

      std::cout << "Removing all entries in the collection. This may take a while "<<collection<<std::endl;
      mongoInterface.removeAllEntries(collection);
      std::cout << "All entries in the collection "<<collection<<" removed"<<std::endl;
   }

   //Create the status thread if we are in verbose mode
   if( verbose ) {
      std::cout << "Creating "
      <<writeThreads<<" write threads"<<std::endl;

      status = std::thread(StatusThread);
   }  


   std::vector<std::thread> threadVect;
   insertTimer.start();;
   writeStartTime = atl::getTime();

   //Spawn write threads that each write m values
   for( int i = 0; i < writeThreads; i ++ ) {
      threadVect.push_back( std::thread( WriteThread, i, &done));
   }

   //Spawn query controller
   std::thread queryController = (std::thread(QueryController));

   //Use the metaGenerator to create artificial metadata
   for( int i = 0; i < totalWrites; i++ ) {
      JsonBox::Value input = metaGenerator(i);
      inputQ.enqueue( input );

      atl::sleep(writeRate);
   }

   done = true;

   //Wait for all threads to complete
   for( auto it = threadVect.begin(); it != threadVect.end(); it++ ) {
      it->join();
   }

   queryController.join();


   //Join the status thread
   if( verbose ) {
      status.join();
   }
}
