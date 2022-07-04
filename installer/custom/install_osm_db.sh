export OSM_DIR=/data/media/0/osm
export DB_DIR=${OSM_DIR}/db
export GZ_MAP_FILE_NAME=tiwan.tar.xz
export GZ_MAP_FILE=${OSM_DIR}/${GZ_MAP_FILE_NAME}

# Remove legacy compressed map file if existing
rm -rf $GZ_MAP_FILE

# WD
cd $OSM_DIR

# Download map file
wget https://mkumard.synology.me/osm/${GZ_MAP_FILE_NAME}

if [[ "$?" != 0 ]]; then
  echo "Error downloading map file"
else
  echo "Successfuly downloaded map file"
  # Remove current db dir if exisiting
  rm -rf $DB_DIR
  # Decompressing
  tar -vxf ${GZ_MAP_FILE_NAME}
  mv tiwan db
fi
