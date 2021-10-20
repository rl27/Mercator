import torch
from model_data.hyperbolic_generative_model import HyperbolicGenerativeModel
from PIL import Image

from pytorch_pretrained_biggan import (BigGAN, one_hot_from_names, truncated_noise_sample,
                                       save_as_images, convert_to_images, display_in_terminal)
import numpy as np


class PoincareBigGAN(HyperbolicGenerativeModel):
    latent_dim = 128

    def __init__(self):
        self.model = BigGAN.from_pretrained('biggan-deep-128')
        #raise ValueError(type(self.model))

    def generate_image_from_latent_vector(self, v) -> Image:
        
        #noise = np.zeros((1,128))
        #noise[0] = v

        #label = one_hot_from_names('eagle', batch_size=1)
        label = np.zeros((1,1000))

        v = torch.tensor(v, dtype=torch.float).to('cuda').unsqueeze(dim=0)
        label = torch.tensor(label, dtype=torch.float)
        
        #noise = noise.to('cuda')
        label = label.to('cuda')
        self.model.to('cuda')

        with torch.no_grad():
            outputs = self.model.forward(v, label, truncation=1.0)
        outputs = outputs.to('cpu')

        images = convert_to_images(outputs)
        return images[0]



    def generate_multiple(self, coords) -> Image:
        num = len(coords)
        
        noise = np.zeros((num,128))
        for i in range(num):
            noise[i,0:64] = coords[i][0]
            noise[i,64:128] = coords[i][1]
        noise += truncated_noise_sample(batch_size=num, dim_z=128, truncation=0.3)*1.5

        label = one_hot_from_names('dog', batch_size=num)

        noise = torch.tensor(noise, dtype=torch.float)
        label = torch.tensor(label, dtype=torch.float)
        noise = noise.to('cuda')
        label = label.to('cuda')
        self.model.to('cuda')

        with torch.no_grad():
            outputs = self.model(noise, label, truncation=1.0)
        outputs = outputs.to('cpu')

        images = convert_to_images(outputs)
        return images