#!/bin/bash

if [ -z "$1" ]
  then
    echo "No program specified"
fi
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $1
