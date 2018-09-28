#!/bin/bash
# A simple demo script that executes effects in sequence, should be precompiled
# and the server running

## declare an array variable
runner="./build/effect_runner.out"
declare -a arr=("$runner temp_timelapse" "$runner temp" "$runner windspeed")
delay=5

while true
do
  for i in "${arr[@]}"
  do
     echo ""
     echo "============================="
     echo "Running $i for $delay seconds"
     echo "============================="
     echo ""
     echo ""
     # Launch script in background
     $i > /dev/null &
     # Get its PID
     PID=$!
     # Wait for $delay seconds
     sleep $delay
     # Kill it
     kill $PID
  done
done
