#!/bin/bash

DIR_REDWOOD="/rwproject/kdd-db/hliubs/spatial/3dor/redwood"
DIR_RSTREE="/rwproject/kdd-db/hliubs/spatial/3dor/rstree"

cd out/

./pre_index.out "$DIR_REDWOOD"/redwood-01-hotel.ply "$DIR_RSTREE"/redwood-01-hotel.rstree
# ./pre_index.out "$DIR_REDWOOD"/redwood-02-d507.ply "$DIR_RSTREE"/redwood-02-d507.rstree
# ./pre_index.out "$DIR_REDWOOD"/redwood-03-living.ply "$DIR_RSTREE"/redwood-03-living.rstree