/**
 * \file MongoAPITest.cpp
 *
 * Created on: June 19, 2017
 * Author: Steve Feller/Nils Persson
 **/
#include <JsonBox.h>
#include <string>

#include "mongoapi.h"
#include "revision.h"




/**
 * \brief Prints the help information
 **/
void printHelp()
{
   std::cout << "MongoTool command line mongo interface application" << std::endl;
//   std::cout << "Version:"<<VERSION<<"\n"<<std::endl;
   std::cout << "options: "<<std::endl;
   std::cout << "\t-c\tcollection to write data to"<<std::endl;
   std::cout << "\t-d\tname of the database"<<std::endl;
   std::cout << "\t-e\terase the specified records"<<std::endl;
   std::cout << "\t-f\tfile to load as data"<<std::endl;
   std::cout << "\t-h\tprint this help menu"<<std::endl;
   std::cout << "\t-i\tinsert the specified record"<<std::endl;
   std::cout << "\t-q\tquery based on provided data"<<std::endl;
   std::cout << "\t-s\tspecify a json string to use as data"<<std::endl;
   std::cout << "\t-uri\tURI of the database in the form of <IP/hostname>:port"<<std::endl;
   std::cout << std::endl;
}

/**
 * \brief Main function
 **/
int main(int argc, char * argv[] ) 
{
   std::string uri = "127.0.0.1:27017";
   std::string collection;
   std::string dbase;
   JsonBox::Value data;

   //Operations
   bool erase = false;
   bool insert = false;
   bool query = false;


   //Add some command line options here
   for( int i = 0; i < argc; i++ ) {
      if( !strcmp(argv[i], "-c")) {
         if( argc <= i++ ) {
            std::cout << "The -c option required a value!"<<std::endl;
            printHelp();
            return 0;
         }
         else {
            collection = argv[i];
         }
      } else if( !strcmp(argv[i], "-d" )) {
         if( argc <= i++ ) {
            std::cout << "The -d option required a value!"<<std::endl;
            printHelp();
            return 0;
         }
         else {
            dbase = argv[i];
         }
      } else if( !strcmp(argv[i], "-e" )) {
         erase = true;
      } else if( !strcmp(argv[i], "-f" )) {
         if( argc <= i++ ) {
            std::cout << "The -f option requires a value!"<<std::endl;
            printHelp();
            return 0;
         }
         if( !data.isNull() ) {
            std::cout << "Value already set when the -f is parsed!"<<std::endl;
            printHelp();
            return 0;
         }
         else {
            data.loadFromFile( argv[i] );
         }
      } else if(!strcmp(argv[i], "-h")) {
         printHelp();
         return 0;
      } else if( !strcmp(argv[i], "-i" )) {
         insert = true;
      } else if( !strcmp(argv[i], "-q" )) {
         query = true;
      } else if( !strcmp(argv[i], "-s" )) {
         if( argc <= i++ ) {
            std::cout << "The -f option requires a value!"<<std::endl;
            printHelp();
            return 0;
         }
         if( !data.isNull() ) {
            std::cout << "Value already set when the -f is parsed!"<<std::endl;
            printHelp();
            return 0;
         }
         else {
            data.loadFromString( argv[i] );
         }
      } else if( !strcmp(argv[i], "-uri" )) {
         if( argc <= i++ ) {
            std::cout << "The -uri option required a value!"<<std::endl;
            printHelp();
            return 0;
         }
         else {
            uri = argv[i];
         }
      }
   }


   //Connect to the database
   mongoapi::MongoInterface mongoInterface(uri);
   bool connected = mongoInterface.connect(dbase);
   if(!connected){
      std::cout << "Unable to connect to server. Exiting" << std::endl;
      return 0;
   }

   JsonBox::Value result;
   //Write the record
   if( insert ) {
      std::cout << "Inserting data into "<<dbase<<"/"<<collection<<std::endl;
      std::cout << data <<std::endl;
      mongoInterface.insert( collection, data);
   }

   //Write the record
   else if( query ) {
      std::cout << "Query "<<dbase<<"/"<<collection<<std::endl;
      std::cout << data <<std::endl;
      result = mongoInterface.query( collection, data);
      result.writeToFile("out.json");
   }
   //Write the record
   else if( erase ) {
      std::cout << "Erase "<<dbase<<"/"<<collection<<std::endl;
      std::cout << data <<std::endl;
      mongoInterface.removeEntries( collection, data);
   }
}
