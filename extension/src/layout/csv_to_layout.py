import csv

spacing = 0.10
mappings = []
pixels = []
strip = []
width = 53
height = 24
x_offset = -26
y_offset = -12
with open('layout/csv/Wiring.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile)
    y = 0
    count = 0
    for row in list(reader):
        x = 0
        for cell in list(row):
            if str(cell) == "":
                x += 1
                #ie. no pixel in this cell
                continue
            pixels.append('\t{"point": [%.2f, %.2f, %.2f]}' %
                          ((width - x + x_offset) * spacing, 0, (height - y + y_offset) * spacing))
            mappings.append('%d %d %d' % (x, y, count))
            cell = str(cell).split("-");
            print(cell)
            strip.append('%s %s %d' % (cell[0], cell[1], count))
            count+=1
            x += 1
        y += 1
    with open('layout/strip_config.txt', 'w') as sf:
        sf.write('\n'.join(strip))
    with open('layout/CoordsToListPos.txt', 'w') as ci:
        ci.write('\n'.join(mappings))
    with open('layout/WorldMap.json', 'w') as ci:
        ci.write('[\n' + ',\n'.join(pixels) + '\n]');
