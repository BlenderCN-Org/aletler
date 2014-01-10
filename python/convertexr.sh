sourcedir="/Users/phaedon/fallingJet/smoothexr/"
destdir="/Users/phaedon/fallingJet/tiffs/"
cd "$sourcedir"
for i in *.exr; do
  ~/pbrt-v2/src/bin/exrtotiff "$i" "$destdir/${i%.*}.tiff"
done
