#!/usr/bin/env sh

make CFLAGS=-DCONFIG_KJSON_SMALLEST=0 clean test || exit 1
make CFLAGS=-DCONFIG_KJSON_SMALLEST=1 clean test || exit 1

echo "All tests passed!"
exit 0
