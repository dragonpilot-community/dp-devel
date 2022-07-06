export OSM_DIR=/data/media/0/osm
export DB_DIR=${OSM_DIR}/db
export XZ_MAP_FILE_NAME=florida.tar.xz
export XZ_MAP_FILE=${OSM_DIR}/${XZ_MAP_FILE_NAME}

# Remove legacy compressed map file if existing
rm -rf $XZ_MAP_FILE

# WD
cd $OSM_DIR

# Download map file
wget -O ${XZ_MAP_FILE_NAME} "https://thecorgirosie-my.sharepoint.com/:u:/p/jason_wen/EfSUnV9PtLFCjDiTrvMZOPIBVeg1SYjMQOCq-FsoUrv5vg?e=yaDpL2&download=1"

if [[ "$?" != 0 ]]; then
  echo "Error downloading map file"
else
  echo "Successfuly downloaded map file"
  # Remove current db dir if exisiting
  rm -rf $DB_DIR
  # Decompressing
  tar -vxf ${XZ_MAP_FILE_NAME}
  mv florida  db
fi
