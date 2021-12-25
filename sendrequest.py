
import sys
# import numpy as np
import requests
import json


num_world_tiles = int(sys.argv[1])

world_data = []
for i in range(num_world_tiles):
    world_data.append([int(sys.argv[3*i+2]), float(sys.argv[3*i+3]), float(sys.argv[3*i+4])])


num_coords = int((len(sys.argv) - 2 - 3*num_world_tiles) / 2)

set_of_coords = []
for i in range(num_coords):
    k = 2 + 3*num_world_tiles
    set_of_coords.append([float(sys.argv[2*i+k]), float(sys.argv[2*i+k+1])])


inp = {'world': world_data, 'coords': set_of_coords}

response = requests.get('http://127.0.0.1:5555/get_image', params={'data': json.dumps(inp)})