import sys
sys.path.append('/Users/Matt/Imperial/120 - C/arm11_22/openpixelcontrol/python')
import opc
import time

# Create a client object
client = opc.Client('localhost:7890')

# Test if it can connect (optional)
if client.can_connect():
    print('connected to...')
else:
    # We could exit here, but instead let's just print a warning
    # and then keep trying to send pixels in case the server
    # appears later
    print('WARNING: could not connect to ...')

# Send pixels forever at 30 frames per second
col = (28, 147, 78)
pixels = [col]
i = 0
time.sleep(2)
while i < 475:
    i = i + 1
    pixels = pixels + [col]
    if client.put_pixels(pixels, channel=0):
        print('...')
    else:
        print('not connected')
    time.sleep(0.01)
