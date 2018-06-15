import csv

spacing = 0.10
mappings = []
pixels = []
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
            pixels.append('{"point": [%.2f, %.2f, %.2f]}' %
                          (x * spacing, 0, y * spacing))
            mappings.append('%d %d %d' % (x, y, count))
            count+=1
    with open('CoordsToListPos.txt', 'w') as ci:
        ci.write('\n'.join(mappings))