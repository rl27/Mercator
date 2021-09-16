#!..\venv\Scripts\python.exe

import sys
import os.path
import pickle
import numpy as np
from image_sampler.ImageSampler import ImageSampler

filename = "image_sampler.pkl"
if os.path.exists(filename):
    print("exists")
    with open(filename, 'rb') as sampler_file:
        sampler = pickle.load(sampler_file)
else:
    print("not exists")
    sampler = ImageSampler()

#x, y = float(sys.argv[1]), float(sys.argv[2])
#x, y = 0.2, 0.3
#set_of_coords = [[x,y]]

num_coords = int((len(sys.argv) - 1) / 2)

set_of_coords = []
for i in range(num_coords):
    set_of_coords.append(np.array([float(sys.argv[2*i+1]), float(sys.argv[2*i+2])]))

sampler.generate_images_for_megatile(set_of_coords)

with open(filename, 'wb') as sampler_file:
    pickle.dump(sampler, sampler_file)