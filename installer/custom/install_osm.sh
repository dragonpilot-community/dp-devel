# OSM Installation instructions:
# https://wiki.openstreetmap.org/wiki/Overpass_API/Installation

# Install expat. All other needed libraries are already installed.
# g++ make expat libexpat1-dev zlib1g-dev
#sudo apt-get update
#sudo apt-get install expat

# Add required path variables to enviroment
export EXEC_DIR=/data/media/0/osm/v0.7.57/
export SOURCE_FILE_ROOT=osm-3s_v0.7.57
export XZ_FILE=${SOURCE_FILE_ROOT}.tar.xz

# Download and extract overpass libary
cd /data/media/0/
mkdir osm
cd /data/media/0/osm
cp -f /data/openpilot/selfdrive/mapd/assets/$XZ_FILE .
tar -vxf $XZ_FILE
mv osm-3s_v0.7.57 v0.7.57
