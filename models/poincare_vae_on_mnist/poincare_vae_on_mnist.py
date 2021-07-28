import os
from os.path import join
from pvae.models.mnist import Mnist
from pvae.utils import get_mean_param
import torch
from torchvision.utils import save_image
import torchvision.transforms.functional as F
from models.hyperbolic_generative_model import HyperbolicGenerativeModel
from collections import namedtuple
import json
from torchvision.utils import make_grid
from PIL import Image


class PoincareVAEonMNIST(HyperbolicGenerativeModel):
    model_args_path = './models/poincare_vae_on_mnist/args.rar'
    model_params_path = './models/poincare_vae_on_mnist/model.rar'

    def __init__(self):
        args = torch.load(PoincareVAEonMNIST.model_args_path)
        # args = json.loads(open(PoincareVAEonMNIST.model_args_path, 'r').read())
        # args = namedtuple("ObjectName", args.keys())(*args.values())
        # args = eval(Poin)
        self.model = Mnist(args)
        self.model.load_state_dict(torch.load(PoincareVAEonMNIST.model_params_path))
        self.model.eval()

    def generate_image_from_coords(self, coords):
        with torch.no_grad():
            px_z_params = self.model.dec(torch.tensor(coords).unsqueeze(dim=0).unsqueeze(dim=0))
            img_tensor = get_mean_param(px_z_params).squeeze(dim=0).squeeze(dim=0)
        grid = make_grid(img_tensor.data.cpu())
        # Add 0.5 after unnormalizing to [0, 255] to round to nearest integer
        ndarr = grid.mul(255).add_(0.5).clamp_(0, 255).permute(1, 2, 0).to('cpu', torch.uint8).numpy()
        im = Image.fromarray(ndarr)
        return im


im = PoincareVAEonMNIST().generate_image_from_coords([-0.3, -0.4])
im.save('lol.png', "PNG")
