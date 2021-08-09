#!C:\Users\raymo\source\repos\HyperViz\venv\Scripts\python.exe

import sys
from models.poincare_vae_on_mnist.poincare_vae_on_mnist import PoincareVAEonMNIST

im = PoincareVAEonMNIST().generate_image_from_coords([float(sys.argv[1]), float(sys.argv[2])])
im.save(sys.argv[3] + ".png", "PNG")