export OSM_DIR=/data/media/0/osm
export DB_DIR=${OSM_DIR}/db
export XZ_MAP_FILE_NAME=us-west.tar.xz
export XZ_MAP_FILE=${OSM_DIR}/${XZ_MAP_FILE_NAME}

# Remove legacy compressed map file if existing
rm -rf $XZ_MAP_FILE

# WD
cd $OSM_DIR

# Download map file
wget -O ${XZ_MAP_FILE_NAME} "https://thecorgirosie-my.sharepoint.com/:u:/p/jason_wen/EYWdXcNpOhJCuOLymUHSwH4BwdRddfiYEbZAK1ltd-hZJA?e=TW5ia4&download=1"

if [[ "$?" != 0 ]]; then
  echo "Error downloading map file"
else
  echo "Successfully downloaded map file"
  # Remove current db dir if existing
  rm -rf $DB_DIR
  # Decompressing
  tar -vxf ${XZ_MAP_FILE_NAME}
  #mv us-west  db

  # Remove compressed map files after expanding
  # rm -rf $GZ_MAP_FILE_NAME
fi
