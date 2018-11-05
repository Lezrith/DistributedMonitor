#!/bin/bash
NODES=`awk -F" " '{if ($1) print $1}' $1`
PWD=`pwd`
for NODE in $NODES
do
    x-terminal-emulator -e bash -c  "./DistributedMonitor $1 $NODE ; read -p 'Press enter to continue'"
done
