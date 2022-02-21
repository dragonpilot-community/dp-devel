export OSM_DIR=/data/osm
export DB_DIR=${OSM_DIR}/db
export GZ_MAP_FILE_NAME=db.tar.gz
export GZ_MAP_FILE=${OSM_DIR}/${GZ_MAP_FILE_NAME}

# Remove legacy compressed map file if existing
[ -f $GZ_MAP_FILE ] && rm -rf $GZ_MAP_FILE

# WD
cd $OSM_DIR

# Download map file
wget https://s3.eu-central-1.amazonaws.com/files.as.osm/${GZ_MAP_FILE_NAME}

if [[ "$?" != 0 ]]; then
  echo "Error downloading map file"
else
  echo "Successfuly downloaded map file"
  # Remove current db dir if exisiting
  [ -f $DB_DIR ] && rm -rf $DB_DIR
  # Uncompress
  tar -zxvf db.tar.gz
  # Remove compressed map files after expanding
  [ -f $GZ_MAP_FILE ] && rm -rf $GZ_MAP_FILE
fi
