import openpixelcontrol.python.opc
import time

# Create a client object
client = openpixelcontrol.python.opc.Client('localhost:7890')

# Test if it can connect (optional)
if client.can_connect():
    print('connected to...')
else:
    # We could exit here, but instead let's just print a warning
    # and then keep trying to send pixels in case the server
    # appears later
    print('WARNING: could not connect to ...')

# Send pixels forever at 30 frames per second
col = (0, 0, 255)
pixels = [(0, 0, 255)]
while True:
    pixels = pixels + [col]
    if client.put_pixels(pixels, channel=0):
        print('...')
    else:
        print('not connected')
    time.sleep(0.01)
