#!/bin/bash
cd `dirname $0`

python3 manager.py stop_strategy $*
