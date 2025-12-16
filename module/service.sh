#!/bin/sh
while [ -z "$(getprop sys.boot_completed)" ]; do
	sleep 5
done
BKT
