sourcedir="/Users/phaedon/github/aletler/meshes/xmls/"
destdir="/Users/phaedon/github/aletler/meshes/tiffs/"
cd "$sourcedir"
for i in *.exr; do
  ~/pbrt-v2/src/bin/exrtotiff "$i" "$destdir/${i%.*}.tiff"
done
