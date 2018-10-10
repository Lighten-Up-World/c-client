#!/bin/bash
# A simple demo script that executes effects in sequence,
# effect_runner should be precompiled and the opc server running

cd /home/pi/c-client/src

runner="build/effect_runner.out"
delay=5
basic_cmd_port=9091

$runner &

while true
do
    # Currently commands has 7 items
    for i in {8..9};
    do
        # Open netcat connection and send a command
        sleep $delay
        echo $i | nc -N localhost $basic_cmd_port
    done
done
