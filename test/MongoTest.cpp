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
unsigned int writeThreads = 20;
int writesPerThread = 100000;
double writeRate = 0.0;
std::string uri = "127.0.0.1:27017";
int verbose = 0;
uint64_t querySuccess = 0;
uint64_t queryFail = 0;


mongoapi::MongoInterface * mongoInterface = NULL;

/**
 * \brief Thread to write multiple records to a databasea
 * \param [in] start initial number to write
 * \param [in] count number of items to write
 * \param [in] tid number of the thread
 * \param [in] collection name of the collection to use
 *
 * This function writes count records to a database. The first record calculated
 * as the thread id * the number of records per 
 **/
void writeThread( int start, int count , unsigned int tid, std::string collection ) {

   //Create record
   JsonBox::Value value;
   value["tid"].setInteger(tid);
   value["timestamp"].setDouble(atl::getTime());

   //Loop through and write records
   for( int i = 0; i < count; i++ ) {
      value["id"].setInteger( start+i );

      std::cout << "Writing: "<<value<<std::endl;
      std::string out = mongoInterface->insert(collection, value );
      if(writeRate > 0 ) {
         atl::sleep(writeRate);
      }
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
void queryThread( int start, int count, unsigned int tid, std::string collection ) {

   for( int i = 0; i < count; i ++ ) {
      JsonBox::Value query;
      query["tid"] = JsonBox::Value(std::to_string((int)tid));
      query["id"] = JsonBox::Value(std::to_string(start+i));
 
      std::cout << "Looking for: "<<query<<std::endl;
      


      JsonBox::Value value = mongoInterface->query(collection,query);
      if( value.getArray().empty()) {
         if( verbose ) {
            std::cout << "query"<<i<<"("<<queryFail<<") Failed Looking for: "<<query<<" in "<<collection<<std::endl;
         }
         queryFail++;
         atl::sleep(.5);
      }
      else {
          querySuccess++;
      }
   }
}

/**
 * \brief Prints the command line help
 **/
void printHelp()
{
   std::cout << "Database testing application\n"<<std::endl;
   std::cout << "option:"<<std::endl;
   std::cout << "\t-h            print this help menu"<<std::endl;
   std::cout << "\t-wt  <int>    number of write threads (default="<<writeThreads<<")"<<std::endl;
   std::cout << "\t-wpt <int>    writes per thread (default="<<writesPerThread<<")"<<std::endl;
   std::cout << "\t-wr  <double> write rate where 0.0 = max (default="<<writeRate<<")"<<std::endl;
   std::cout << "\t-v            enter verbose mode (default="<<verbose<<")"<<std::endl;
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

   //Parse command line options
   int i = 1;
   while( i < argc ) {
      if( !strcmp( argv[i], "-h")) {
         printHelp();
         exit(1);
      }
      else if( !strcmp( argv[i], "-wpt")) {
         i++;
         writesPerThread = std::stoi(argv[i]);
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
         verbose = 1;
      }
      else {
         std::cout <<"Invalid command: "<<argv[i]<<". Run with the -h to see options\n\n"<<std::endl;
         
      }

      i++;
   }


   //Try to connect to the databse
   mongoInterface =  new mongoapi::MongoInterface(uri);
   bool connected = mongoInterface->connect("test");
   if(!connected){
      std::cout << "Unable to connect to the database. Exiting" << std::endl;
      exit(1);
   }

   if( verbose ) {
      std::cout << "Creating "
                <<writeThreads<<" threads that will each write "
                <<writesPerThread<<" records for a total of " 
                <<writesPerThread * writeThreads<<" entries"<<std::endl;

      std::cout << "This may take several seconds\n"<<std::endl;
   }  


   //Spawn n write threads that each write m values
   std::vector<std::thread> threadVect;
   atl::Timer timer;

   for( unsigned int i = 0; i < writeThreads; i ++ ) {
      threadVect.push_back( std::thread( writeThread, i*writesPerThread, writesPerThread, i, "test" ));
   }


   //Wait for all threads to complete
   for( auto it = threadVect.begin(); it != threadVect.end(); it++ ) {
      it->join();
   }

   size_t writeCount = writeThreads * writesPerThread;
   double elapsed = timer.elapsed();
   double rate = writeCount / elapsed;

   std::cout << writeCount << " records written in "<<elapsed<<" seconds ("<<rate<<"fps) or "<<writesPerThread/elapsed<<" fps/thread"<<std::endl;


   std::cout << "Direct Queries to check" <<std::endl;
   atl::Timer queryTimer;

  
   //Spawn query threads 
   for( unsigned int i = 0; i < 1; i ++ ) {
      threadVect.push_back( std::thread( queryThread, i*writesPerThread, writesPerThread, i, "test" ));
   }

   elapsed = queryTimer.elapsed();
   double totalQueries = querySuccess+queryFail;
   double percent = 0;
   if( totalQueries != 0 ) {
      std::cout << "Succeeded in "<<querySuccess<<" of "<<totalQueries<<" or "<< double(100*querySuccess)/totalQueries<<"%"<<" in "<<elapsed<<" seconds."<<std::endl;
   }

   delete mongoInterface;
}
