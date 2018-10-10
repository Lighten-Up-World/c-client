#!/bin/bash
# A simple demo script that executes effects in sequence,
# effect_runner should be precompiled and the opc server running

cd /home/pi/c-client/src

runner="build/effect_runner.out"
delay=20
basic_cmd_port=9091

$runner >/dev/null &

while true
do
    # Currently commands has 7 items
    for i in {0..5};
    do
        # Open netcat connection and send a command
        echo $i | nc -w0 localhost $basic_cmd_port
        sleep $delay
    done
done
