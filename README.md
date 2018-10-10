# ARM11 Project Extension: "Lighten up World"
An LED World Map displaying data from a range of APIs and visual effects

![LED MAP](https://github.com/Lighten-Up-World/c-client/raw/master/images/LED%20Board.jpg)

**Prerequistes:** If you're using Linux, first get the dependencies you need
(Mac users skip this step):

    apt-get install mesa-common-dev freeglut3-dev


## Simulation instructions

First start the simulation server by running
    make simulation

## Run instructions (must be run from within the Raspberry Pi controller)

The Raspberry Pi should have been set up such that fcserver runs on startup.
Then to display on the map, run:
    make run

