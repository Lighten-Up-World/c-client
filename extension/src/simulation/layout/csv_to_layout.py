#!/usr/bin/env python
import csv

with open('WorldMap.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile)
    width = 52
    height = 24
    x_offset = -width / 2
    y = 0
    count = 0
    spacing = 0.10  # m
    pixels = []
    mappings = []
    for row in reversed(list(reader)):
        x = -1 + x_offset
        for cell in reversed(list(row)):
            x+=1
            if not cell:
                continue
            count+=1
            pixels.append('  {"point": [%.2f, %.2f, %.2f]}' %
                         (x*spacing, 0, y*spacing))
            mappings.append('%d %d %d' % (x + width , height-y, count))

        y+=1
    with open('WorldMap.json', 'w') as out:
         out.write('[\n' + ',\n'.join(pixels) + '\n]')
    with open('CoordInteger.txt', 'w') as ci:
        ci.write('\n'.join(mappings))
    print('Written: ' + str(count) + ' pixels')
