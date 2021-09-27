import torch
from model_data.hyperbolic_generative_model import HyperbolicGenerativeModel
from PIL import Image

from pytorch_pretrained_biggan import (BigGAN, one_hot_from_names, truncated_noise_sample,
                                       save_as_images, convert_to_images, display_in_terminal)
import numpy as np


class PoincareBigGAN(HyperbolicGenerativeModel):
    latent_dim = 2

    def __init__(self):
        self.model = BigGAN.from_pretrained('biggan-deep-128')

    def generate_image_from_latent_vector(self, v) -> Image:
        coords = v
        
        label = one_hot_from_names('bicycle', batch_size=1)
        noise = np.zeros((1,128))
        noise[:,0:64] = coords[0]
        noise[:,64:128] = coords[1]

        noise = torch.tensor(noise, dtype=torch.float)
        label = torch.tensor(label, dtype=torch.float)
        with torch.no_grad():
            outputs = self.model(noise, label, truncation=0.5)

        images = convert_to_images(outputs)
        return images[0]