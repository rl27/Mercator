#!/usr/bin/env python

import torch
from model_data.hyperbolic_generative_model import HyperbolicGenerativeModel
from PIL import Image
import numpy as np

class PoincareGANzoo(HyperbolicGenerativeModel):
    # 120 for DCGAN, 512 for PGAN
    latent_dim = 512

    def __init__(self):
        # PGAN models: ['celebAHQ-256', 'celebAHQ-512', 'DTD', 'celeba']
        use_gpu = True if torch.cuda.is_available() else False
        self.model = torch.hub.load('facebookresearch/pytorch_GAN_zoo:hub',
                                    'PGAN', model_name='celebAHQ-256',
                                     pretrained=True, useGPU=use_gpu)


    def generate_image_from_latent_vector(self, v) -> Image:
        
        # v = np.array(v)
        # v = v / np.sqrt(np.sum(v**2))

        #v = v / np.sqrt(np.sum(v**2, axis=1, keepdims=True))

        v = torch.tensor(v, dtype=torch.float).to('cuda').unsqueeze(dim=0)

        with torch.no_grad():
            outputs = self.model.test(v, toCPU=True)

        images = self.convert_to_images(outputs)
        return images[0]


    def convert_to_images(self, obj):
        """ Convert an output tensor from BigGAN in a list of images.
            Params:
                obj: tensor or numpy array of shape (batch_size, channels, height, width)
            Output:
                list of Pillow Images of size (height, width)
        """
        try:
            import PIL
        except ImportError:
            raise ImportError("Please install Pillow to use images: pip install Pillow")

        if not isinstance(obj, np.ndarray):
            obj = obj.detach().numpy()

        obj = obj.transpose((0, 2, 3, 1))
        obj = np.clip(((obj + 1) / 2.0) * 256, 0, 255)

        img = []
        for i, out in enumerate(obj):
            out_array = np.asarray(np.uint8(out), dtype=np.uint8)
            img.append(PIL.Image.fromarray(out_array))
        return img
