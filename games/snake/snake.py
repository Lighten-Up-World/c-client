# SNAKE GAME
# Use ARROW KEYS to play, SPACE BAR for pausing/resuming and Esc Key for exiting

import curses
from curses import KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN
from random import randint


## Think these may be needed for simulation
#-------------------------------------------------------------------------------
# parse layout file

#print
#print '    parsing layout file'
#print

#coordinates = []
#for item in json.load(open(options.layout)):
#    if 'point' in item:
#        coordinates.append(tuple(item['point']))


#-------------------------------------------------------------------------------
# connect to server

#client = opc.Client(options.server)
#if client.can_connect():
#    print '    connected to %s' % options.server
#else:
    # can't connect, but keep running in case the server appears later
    #print '    WARNING: could not connect to %s' % options.server
#print


#-------------------------------------------------------------------------------

curses.initscr()
win = curses.newwin(20, 60, 0, 0)
win.keypad(1)
curses.noecho()
curses.curs_set(0)
win.border(0)
win.nodelay(1)

# Initialise values
key = KEY_RIGHT
score = 0

# Initial snake co-ordinates
snake = [[4,10], [4,9], [4,8]]
# First food co-ordinates
food = [10,20]

# Print food
win.addch(food[0], food[1], '*')

# While Esc key is not pressed
while key != 27:
    win.border(0)
    # Print'Score' and 'SNAKE' strings
    win.addstr(0, 2, 'Score : ' + str(score) + ' ') # TO BE SHWON IN WEBAPP
    # Increase speed of Snake as length increases
    win.timeout(150 - (len(snake)/5 + len(snake)/10)%120)

    # Previous key pressed
    prevKey = key
    event = win.getch()
    key = key if event == -1 else event


    # If SPACE BAR is pressed, wait for another one (Pause/Resume)
    if key == ord(' '):
        key = -1
        while key != ord(' '):
            key = win.getch()
        key = prevKey
        continue

    # If an invalid key is pressed
    if key not in [KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, 27]:
        key = prevKey

    # Calculates the new coordinates of the head of the snake. NOTE: len(snake) increases.
    # This is taken care of later at [1].
    snake.insert(0, [snake[0][0] + (key == KEY_DOWN and 1) + (key == KEY_UP and -1), snake[0][1] + (key == KEY_LEFT and -1) + (key == KEY_RIGHT and 1)])

    # If snake crosses the boundaries, make it enter from the other side
    if snake[0][0] == 0: snake[0][0] = 18
    if snake[0][1] == 0: snake[0][1] = 58
    if snake[0][0] == 19: snake[0][0] = 1
    if snake[0][1] == 59: snake[0][1] = 1

    # Exit if snake crosses the boundaries (Uncomment to enable)
    #if snake[0][0] == 0 or snake[0][0] == 19 or snake[0][1] == 0 or snake[0][1] == 59: break

    # If snake runs over itself
    if snake[0] in snake[1:]: break


    # When snake eats the food
    if snake[0] == food:
        food = []
        score += 1
        while food == []:
            # Calculate next food's coordinates
            food = [randint(1, 18), randint(1, 58)]
            if food in snake: food = []
        win.addch(food[0], food[1], '*')
    else:
        # If it does not eat food, length decreases
        last = snake.pop()
        win.addch(last[0], last[1], ' ')
    win.addch(snake[0][0], snake[0][1], '#')

curses.endwin()
print("\nScore - " + str(score))
print("GAME OVER") # TO BE SHOWN ON WEBAPP
