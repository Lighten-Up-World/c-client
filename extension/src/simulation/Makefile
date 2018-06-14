OPEN_PIXEL_CONTROL_PATH = ../../../openpixelcontrol/

simulate: layout
	$(OPEN_PIXEL_CONTROL_PATH)bin/gl_server -l layout/WorldMap.json
	python fillAll.py

layout:
	python layout/csv_to_layout.py
