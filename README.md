# ARM11 Project Extension: "Lighten up World"
An LED World Map displaying data from a range of APIs and visual effects

**Prerequistes:** If you're using Linux, first get the dependencies you need
(Mac users skip this step):

    apt-get install mesa-common-dev freeglut3-dev


## Simulation instructions

First start the simulation server by running
    make simulation MODE=$(MODE)
replacing $(MODE) with the desired mode (see below)


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
