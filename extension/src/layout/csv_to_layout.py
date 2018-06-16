import csv

spacing = 0.10
mappings = []
pixels = []
width = 53
height = 24
x_offset = -26
y_offset = -12
with open('WorldMap.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile)
    y = 0
    count = 0
    for row in list(reader):
        x = 0
        y+=1
        for cell in list(row):
            x+=1
            if not cell:
                #ie. no pixel in this cell
                continue
            pixels.append('\t{"point": [%.2f, %.2f, %.2f]}' %
                          ((width - x + x_offset) * spacing, 0, (height - y + y_offset) * spacing))
            mappings.append('%d %d %d' % (x, y, count))
            count+=1
    with open('CoordsToListPos.txt', 'w') as ci:
        ci.write('\n'.join(mappings))
    with open('WorldMap.json', 'w') as ci:
        ci.write('[\n' + ',\n'.join(pixels) + '\n]');
