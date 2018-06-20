import csv

MAX_STRIP_SIZE = 64
NUM_CHANNELS = 8
spacing = 0.10
mappings = []
pixels = []
pixel_channels = [["" for n in range(MAX_STRIP_SIZE)] for x in range(NUM_CHANNELS)]
strip = []
width = 53
height = 24
x_offset = -26
y_offset = -12

with open('layout/csv/WorldMap.csv', 'rb') as csvfile:
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
            cell = str(cell).split("-");
            cell_channel = int(cell[0])
            cell_num = int(cell[1])-1

            pixels.append('\t{"point": [%.2f, %.2f, %.2f]}' %
                          ((width - x + x_offset) * spacing, 0, (height - y + y_offset) * spacing))
            pixel_channels[cell_channel][cell_num] = pixels[-1]

            mappings.append('%d %d' % (x, y))
            strip.append('%d %s' % (cell_channel, cell_num))
            count+=1
            x += 1
        y += 1
with open('layout/pixel_config.txt', 'w') as sf:
    sf.write('x y channel num\n')
    sf.write('\n'.join([ m + ' ' + s for m, s in zip(mappings, strip)]))
# with open('layout/strip_config.txt', 'w') as sf:
#     sf.write('\n'.join(strip))
# with open('layout/coordinates.txt', 'w') as ci:
#     ci.write('\n'.join(mappings))
with open('layout/WorldMap.json', 'w') as f:
    f.write('[\n' + ',\n'.join(pixels) + '\n]')

with open('layout/channel_lengths.txt', 'w') as f:
    f.write('')
size = []
for i in range(NUM_CHANNELS):
    stripped_channel = [p for p in pixel_channels[i] if p != ""]
    with open('layout/WorldMap' + str(i+1) + '.json', 'w') as f:
        f.write('[\n' + ',\n'.join(stripped_channel) + '\n]');
    size += [str(len(stripped_channel))]

with open('layout/channel_lengths.txt', 'a') as cl:
    cl.write('\n'.join(size))
