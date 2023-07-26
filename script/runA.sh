#!/bin/bash

echo "cd /Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/code"
cd /Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/code
#make clean
make release
cp code /Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/script/
# Specify the folder path

echo "cd /Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/script/"
cd /Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/script/

folder="/Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/instances/cdp"

for file in "$folder"/A*
do
    # Check if the current item is a file
    if [ -f "$file" ]; then
        echo "Processing file: $file"
        ./code "$file"
    fi
done