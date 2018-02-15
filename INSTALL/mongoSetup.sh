#Mongo installation directions
#
# This script is a sequence of commands used to install the latest community edition of MongoDB onto an 
# Ubuntu 16.04 system. 
#
# Reference: https://docs.mongodb.com/manual/tutorial/install-mongodb-on-ubuntu/

#Please make sure all exising mongodb packages are removed
sudo apt purge mongo*

#create a backup of existing mongo data if it exists(this will delete any database information
#reference: https://bbs.archlinux.org/viewtopic.php?id=233435
sudo mv /var/lib/mongodb /var/lib/mongodb_bak

#Import public key used by the mongo package amangement system
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 2930ADAE8CAF5059EE73BB4B58712A2291FA4AD5

#Create /etc/apt/sources.list.d/mongodb-org-3.6.list for ubuntu16.04
echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/3.6 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.6.list

#reload the package database
sudo apt-get update

#install the mongodb packages
sudo apt-get install -y mongodb-org

#Install a specific release of mongo  (in this case, 3.6.2). Skipping for now
#sudo apt-get install -y mongodb-org=3.6.2 mongodb-org-server=3.6.2 mongodb-org-shell=3.6.2 mongodb-org-mongos=3.6.2 mongodb-org-tools=3.6.2

#Pin a specific mongo version to prevent accidental upgrades
echo "mongodb-org hold" | sudo dpkg --set-selections
echo "mongodb-org-server hold" | sudo dpkg --set-selections
echo "mongodb-org-shell hold" | sudo dpkg --set-selections
echo "mongodb-org-mongos hold" | sudo dpkg --set-selections
echo "mongodb-org-tools hold" | sudo dpkg --set-selections


