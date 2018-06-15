OPEN_PIXEL_CONTROL_PATH = ../../../openpixelcontrol/

simulate: layout
	$(OPEN_PIXEL_CONTROL_PATH)bin/gl_server -l layout/WorldMap.json &
	python fillAll.py

test:
	gcc simulate.c -o simulate
	$(OPEN_PIXEL_CONTROL_PATH)bin/gl_server -l layout/WorldMap.json &
	./simulate

layout:
	python layout/csv_to_layout.py
