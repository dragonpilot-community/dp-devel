# OSM Installation instructions:
# https://wiki.openstreetmap.org/wiki/Overpass_API/Installation

# Install expat. All other needed libraries are already installed.
# g++ make expat libexpat1-dev zlib1g-dev
sudo apt-get update
sudo apt-get install expat

# Add required path variables to enviroment
export EXEC_DIR=/data/osm/v0.7.57/
export SOURCE_FILE_ROOT=osm-3s_v0.7.57
export GZ_FILE=${SOURCE_FILE_ROOT}.tar.gz
# export DB_DIR=/data/osm/db/

# Download and extract overpass libary
cd /data
mkdir osm
cd /data/osm
wget http://dev.overpass-api.de/releases/$GZ_FILE
tar -zxvf $GZ_FILE

# Configure and install overpass
cd $(ls | grep $SOURCE_FILE_ROOT\.[0-9]*)
./configure CXXFLAGS="-O2" --prefix=$EXEC_DIR
make install

# Remove source files after installation
cd ..
rm -rf osm-3s_v0.7.57*
