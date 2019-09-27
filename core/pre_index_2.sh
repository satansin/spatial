#!/bin/bash

DIR_REDWOOD="/rwproject/kdd-db/hliubs/spatial/3dor/redwood"
DIR_RSTREE="/rwproject/kdd-db/hliubs/spatial/3dor/rstree"

cd out/

./pre_index.out "$DIR_REDWOOD"/redwood-03-living.ply "$DIR_RSTREE"/redwood-03-living.rstree