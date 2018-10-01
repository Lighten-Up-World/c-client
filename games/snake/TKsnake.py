from __future__ import division

try:
    # Python2
    import Tkinter as tk
except ImportError:
    # Python3
    import tkinter as tk

from time import sleep
from random import randint

# -------------------------------------------------------------------------------
# imports for displaying on simulation

import optparse
import random

try:
    import json
except ImportError:
    import simplejson as json

import opc

# -------------------------------------------------------------------------------
# SNAKE GAME
# Must run make server first in src dir
# Use ARROW KEYS to play, SPACE BAR for pausing/resuming and Esc Key for exiting

# command line

parser = optparse.OptionParser()
parser.add_option('-l', '--layout', dest='layout',
                  action='store', type='string',
                  help='layout file')
parser.add_option('-s', '--server', dest='server', default='127.0.0.1:22368',
                  action='store', type='string',
                  help='ip and port of server')
parser.add_option('-f', '--fps', dest='fps', default=20,
                  action='store', type='int',
                  help='frames per second')

options, args = parser.parse_args()

if not options.layout:
    parser.print_help()
    print
    print 'ERROR: you must specify a layout file using --layout'
    print
    sys.exit(1)

# -------------------------------------------------------------------------------
# parse layout file

print
print '    parsing layout file'
print

coordinates = []
for item in json.load(open(options.layout)):
    if 'point' in item:
        coordinates.append(tuple(item['point']))

# -------------------------------------------------------------------------------
# connect to server

client = opc.Client(options.server)
if client.can_connect():
    print '    connected to %s' % options.server
else:
    # can't connect, but keep running in case the server appears later
    print '    WARNING: could not connect to %s' % options.server
print


# -------------------------------------------------------------------------------
# function to translate snake coordinates

def translate_snake_coords(snake_coords):
    coords = ((snake_coords[1] - 25) / 10, 0, (snake_coords[0] - 10) / 10)
    return coords


# -------------------------------------------------------------------------------
# Initialise pixels and define colours

WHITE_PIXEL = (255, 255, 255)
SNAKE_COLOUR = (92, 180, 0)
FOOD_COLOUR = (234, 64, 53)

# Pixels to be displayed on world map - (snake and food added later)
pixels = [WHITE_PIXEL for coords in enumerate(coordinates)]

# -------------------------------------------------------------------------------
# keylogger code to monitor keys

def key(event):
    ### shows key or tk code for the key ###
    if event.keysym in ['Left', 'Right', 'Up', 'Down', 'Escape']:
        global currKey
        currKey = event.keysym
    else:
        # If an invalid key is pressed
        global currKey
        currKey = prevKey

# -------------------------------------------------------------------------------
print("Playing snake game - press space to pause, Esc to exit")

HEIGHT = 22
WIDTH = 53

# Initialise values
currKey = 'Right'
score = 0

# TODO: Randomize - might be eaiser for map if fixed though
# Initial snake co-ordinates
snake = [[10, 25], [10, 24], [10, 23]]
# First food co-ordinates
food = [15, 20]

root = tk.Tk()
# While Esc key is not pressed
while currKey != 'Escape':

    root.bind_all('<Key>', key)
    # don't show the tk window
    root.withdraw()

    if currKey == 'Escape':
        break

    # Calculates the new coordinates of the head of the snake. NOTE: len(snake) increases.
    # This is taken care of later.
    snake.insert(0, [snake[0][0] + (currKey == 'Down' and -1)
                     + (currKey == 'Up' and 1),
                     snake[0][1] + (currKey == 'Left' and 1)
                     + (currKey == 'Right' and -1)])

    # If snake crosses the boundaries, make it enter from the other side
    if snake[0][0] == 0: snake[0][0] = HEIGHT - 2
    if snake[0][1] == 0: snake[0][1] = WIDTH - 2
    if snake[0][0] == HEIGHT - 1: snake[0][0] = 1
    if snake[0][1] == WIDTH - 1: snake[0][1] = 1

    # Exit if snake crosses the boundaries (Uncomment to enable)
    # if snake[0][0] == 0 or snake[0][0] == HEIGHT - 1 or snake[0][1] == 0 or
    # snake[0][1] == WIDTH - 1: break

    # If snake runs over itself
    # if snake[0] in snake[1:]: break

    # When snake eats the food
    if snake[0] == food:
        food = []
        score += 1
        while food == []:
            # Calculate next food's coordinates
            food = [randint(1, HEIGHT - 2), randint(1, WIDTH - 2)]
            if food in snake:
                food = []

    else:
        # If it does not eat food, length decreases
        last = snake.pop()
        # remove end of snake so it appears to move
        if (translate_snake_coords(last)) in coordinates:
            pixels[coordinates.index(translate_snake_coords(last))] = \
                WHITE_PIXEL

    # Get map coords of snake and add to pixels
    map_snake_coords = [translate_snake_coords(s_coord) for s_coord in snake]

    for mapped_s_coord in map_snake_coords:
        if mapped_s_coord in coordinates:
            pixels[coordinates.index(mapped_s_coord)] = SNAKE_COLOUR

    # Get map coordinates for food and add to pixels
    food_coord = translate_snake_coords(food)

    if food_coord in coordinates:
        pixels[coordinates.index(food_coord)] = FOOD_COLOUR

    # Display pixels on world map
    client.put_pixels(pixels, channel=0)

    sleep(0.1)


root.destroy()
# everything goes red when dies
pixels = [FOOD_COLOUR for coords in enumerate(coordinates)]
client.put_pixels(pixels, channel=0)
# print("\nScore - " + str(score))

# -------------------------------------------------------------------------------
