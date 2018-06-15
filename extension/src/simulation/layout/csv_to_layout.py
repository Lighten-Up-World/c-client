#!/usr/bin/env python
import csv

with open('WorldMap.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile)
    width = 52
    height = 24
    total_pixels = 472
    x_offset = -width / 2
    pixel_y = 0
    count = 0
    spacing = 0.10  # m
    pixels = []
    mappings = []

    y = 0
    for row in reversed(list(reader)):
        x = 0
        pixel_x = -1 + x_offset
        for cell in reversed(list(row)):
            pixel_x+=1
            x+=1
            if not cell:
                continue
            count+=1
            pixels.append('  {"point": [%.2f, %.2f, %.2f]}' %
                         (pixel_x*spacing, 0, pixel_y*spacing))
            mappings.append('%d %d %d' % (width - x , height - y, count))
        y+=1
        pixel_y+=1
    with open('WorldMap.json', 'w') as out:
         out.write('[\n' + ',\n'.join(pixels) + '\n]')
    with open('CoordsToListPos.txt', 'w') as ci:
        ci.write('\n'.join(mappings))
    print('Written: ' + str(count) + ' pixels')
