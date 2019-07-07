#!/bin/bash
awk -F" |," '{ print $1" "$4 }' $1 | sort -k2 -n |tail