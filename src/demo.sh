#!/bin/bash
# A simple demo script that executes effects in sequence, should be precompiled
# and the server running

cd /home/pi/c-client/src

## declare an array variable
runner="/home/pi/c-client/src/build/effect_runner.out"
declare -a arr=("temp_timelapse" "sun")
declare -a arr2=("raverplaid" "conway" "lavalamp")
delay=20

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
     $runner $i > /dev/null &
     # Get its PID
     PID=$!
     # Wait for $delay seconds
     sleep $delay
     # Kill it
     kill $PID
  done

  for i in "${arr2[@]}"
  do
    echo ""
    echo "============================="
    echo "Running $i for $delay seconds"
    echo "============================="
    echo ""
    echo ""
    # Launch script in background
    $runner $i > /dev/null &
    # Get its PID
    PID=$!
    # Wait for $delay seconds
    sleep $delay
    # Kill it
    kill $PID

    # Get the PID of python and kill it
    ps | grep python | awk '{print $1}' | xargs kill 
  done

done
