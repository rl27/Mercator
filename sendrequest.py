#!..\venv\Scripts\python.exe

import sys
import numpy as np
import requests
import json

num_coords = int((len(sys.argv) - 1) / 2)

set_of_coords = []
for i in range(num_coords):
    set_of_coords.append(np.array([float(sys.argv[2*i+1]), float(sys.argv[2*i+2])]))

response = requests.get('http://127.0.0.1:5555/get_image', params={'coords': json.dumps(np.array(set_of_coords).tolist())})