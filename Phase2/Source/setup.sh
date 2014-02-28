make clean
make > log 2>&1
cat log | grep error
