# ARM11 Project Extension: "Lighten up World"
An LED World Map displaying data from a range of APIs and visual effects

![LED MAP](https://github.com/Lighten-Up-World/c-client/raw/master/images/LED%20Board.jpg)

**Prerequistes:** If you're using Linux, first get the dependencies you need
(Mac users skip this step):

    apt-get install mesa-common-dev freeglut3-dev


## Simulation instructions

First start the simulation server by running
    make simulation MODE=$(MODE)
replacing $(MODE) with the desired mode (see below)

![Timelapse](https://github.com/Lighten-Up-World/c-client/raw/master/images/timelapse.gif)

![Lava Lamp](https://github.com/Lighten-Up-World/c-client/raw/master/images/Lava%20Lamp.gif)

![Temp](https://github.com/Lighten-Up-World/c-client/raw/master/images/Temperature%20Live.gif)

![Scroll](https://github.com/Lighten-Up-World/c-client/raw/master/images/Short%20Scroll.gif)

## Run instructions (must be run from within the Raspberry Pi controller)

The Raspberry Pi should have been set up such that fcserver runs on startup.
Then to display on the map, run:
    make run MODE=$(MODE)
replacing $(MODE) with the desired mode

Available modes are
- temp
- temp_timelapse
- temp_log
- windspeed
- scroll
- temp
- image
- sun
- test
- test_all
