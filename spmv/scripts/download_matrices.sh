#!/usr/bin/sh


urls=(
    "https://suitesparse-collection-website.herokuapp.com/MM/HB/dwt_512.tar.gz"
    "https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/uk.tar.gz"
    "https://suitesparse-collection-website.herokuapp.com/MM/Mulvey/pfinan512.tar.gz"
    "https://suitesparse-collection-website.herokuapp.com/MM/Marini/eurqsa.tar.gz"
)

pushd ../matrices > /dev/null

for url in ${urls[@]}; do
    wget $url
    file=${url##*/}
    tar -xf "$file"
done

rm -rf *.tar.gz*

popd > /dev/null