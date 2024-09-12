#!/bin/bash

set -e
cd `dirname $0`

./stop.sh

python3 manager.py restart_all 

sleep 1

python3 manager.py status

touch /tmp/xyts_running
