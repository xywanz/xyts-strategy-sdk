#!/bin/bash
cd `dirname $0`

rm -f /tmp/xyts_running

python3 manager.py stop_all 
