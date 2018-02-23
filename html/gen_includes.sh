#!/bin/bash

mkdir -p include
for file in *.html; do
	xxd -i $file include/$(echo $file | cut -f 1 -d '.').h
done

