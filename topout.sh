#!/bin/bash
awk -F" |," '{ print $1" "$4 }' out | sort -k2 -n |tail