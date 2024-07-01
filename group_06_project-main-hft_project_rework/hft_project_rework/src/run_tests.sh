#!/usr/bin/bash

for i in {1..12..1}; do
	sudo ../build/./tests $i
done
