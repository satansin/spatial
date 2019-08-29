#!/bin/sh

# dos2unix if it does not work

cd /homes/hliubs/proj_sync/git/spatial/src/out/

# range sub-ply
./range.out -10000 -50 130 10000 -10000 -10

# make it noisy
./noise.out 0.1

# transformation
cd ../../trimesh2/bin.Linux64/
./mesh_filter ../../src/data/t2_noisy.ply -ixform ../../src/data/t2.xf ../../src/data/t2_query.ply