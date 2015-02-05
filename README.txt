To install:

##### TO INSTALL GIT #####
sudo apt-get install git

##### TO INSTALL JSONBOX #####
git clone https://github.com/anhero/JsonBox.git
mkdir JsonBox-build; cd JsonBox-build
cmake ../JsonBox
make
sudo make install; cd ..

##### TO INSTALL BOOST #####
sudo apt-get install libboost1.55

##### TO INSTALL DOXYGEN (OPTIONAL) #####
sudo apt-get install doxygen

##### TO INSTALL MONGO-CXX-DRIVER #####
sudo apt-get install scons
git clone https://github.com/mongodb/mongo-cxx-driver.git
cd mongo-cxx-driver
git checkout legacy
	## this installs inside the /usr/local/mongo directory
sudo scons --prefix=/usr/local install

##### INSTALL DISPCODE_MONGOAPI #####
git clone https://dispcode.unfuddle.com/git/dispcode_mongoapi/
mkdir dispcode_mongoapi-build; cd dispcode_mongoapi-build
cmake ../dispcode_mongoapi
make
sudo make install
cd ..

