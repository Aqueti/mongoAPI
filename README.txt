Recommended install:
Use superbuild.  This is the default setting.

#### Submodule initiation ####
Run "git submodule init"
Update: "git submodule update"

##### For Doxygen, run: #####
sudo apt-get install doxygen
sudo apt-get install graphviz


##################################
##### TO INSTALL MONGODB 3.6 #####
##################################
# remove Ubuntu repository verison of mongo
sudo apt purge mongodb mongodb-org*

# add official mongo repo and install
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv EA312927
echo "deb http://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/3.6 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.6.list
sudo apt-get update
sudo apt-get install mongodb-org

# create systemd service file by pasting the following in a new file at /lib/systemd/system/mongod.service
[Unit]
Description=High-performance, schema-free document-oriented database
After=network.target
Documentation=https://docs.mongodb.org/manual

[Service]
User=mongodb
Group=mongodb
ExecStart=/usr/bin/mongod --quiet --config /etc/mongod.conf
# file size
LimitFSIZE=infinity
# cpu time
LimitCPU=infinity
# virtual memory size
LimitAS=infinity
# open files
LimitNOFILE=64000
# processes/threads
LimitNPROC=64000
# total threads (user+kernel)
TasksMax=infinity
TasksAccounting=false

[Install]
WantedBy=multi-user.target

# start mongo
sudo service mongod start



To install manually:
in the install directory, run ccmake [source_directory] and turn superbuild "off"

##### TO INSTALL BOOST #####
sudo apt-get install libboost-all-dev

##### TO INSTALL SCONS #####
sudo apt-get install scons

##### TO INSTALL GIT #####
sudo apt-get install git

##### TO INSTALL JSONBOX #####
git clone https://github.com/anhero/JsonBox.git
mkdir JsonBox-build; cd JsonBox-build
cmake ../JsonBox
make
sudo make install; cd ..

##### TO INSTALL MONGO-CXX-DRIVER #####
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
