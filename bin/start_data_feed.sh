#!/bin/bash

cd `dirname $0`

python3 manager.py start "shared_memory_data_feed $1"
