#!/bin/bash

for i in $(sudo docker images --filter "dangling=true" -q)
do
    sudo docker rmi -f $i
done
