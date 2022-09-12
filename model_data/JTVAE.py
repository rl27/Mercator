#!/usr/bin/env python

'''
Example RDKit usage:
from rdkit import Chem
from rdkit.Chem.Draw import MolToImage
mol = Chem.MolFromSmiles("C1CC2=C3C(=CC=C2)C(=CN3C1)[C@H]4[C@@H](C(=O)NC4=O)C5=CNC6=CC=CC=C65")
im = MolToImage(mol)
im.save("testing.png")

#from rdkit.Chem.Draw import MolToFile
#MolToFile(mol, "testing.png")
'''

import torch
from model_data.hyperbolic_generative_model import HyperbolicGenerativeModel
from PIL import Image
import numpy as np

from rdkit import Chem
from rdkit.Chem.Draw import MolToImage

import sys
sys.path.insert(0,'model_data/fast_jtnn')
from model_data.fast_jtnn import *

import rdkit

lg = rdkit.RDLogger.logger() 
lg.setLevel(rdkit.RDLogger.CRITICAL)

class PoincareJTVAE(HyperbolicGenerativeModel):

    latent_dim = 28

    def __init__(self):
        vocab_file = 'model_data/vocab.txt'
        vocab = [x.strip("\r\n ") for x in open(vocab_file)] 
        vocab = Vocab(vocab)

        hidden_size = 450
        latent_size = self.latent_dim * 2
        depthT = 20
        depthG = 3
        model = 'model_data/model.iter-400000'

        self.model = JTNNVAE(vocab, hidden_size, latent_size, depthT, depthG)
        self.model.load_state_dict(torch.load(model))
        self.model = self.model.cuda()


    def generate_image_from_latent_vector(self, v) -> Image:

        v = torch.tensor(v, dtype=torch.float).to('cuda')

        with torch.no_grad():
            output = self.model.sample_from_v(v)

        return MolToImage(Chem.MolFromSmiles(output))

    def generate_multiple(self, v) -> Image:

        v = torch.tensor(v, dtype=torch.float).to('cuda')

        outputs = []
        with torch.no_grad():
            for v1 in v:
                outputs.append(self.model.sample_from_v(v1.unsqueeze(dim=0)))

        images = []
        for smiles in outputs:
            mol = Chem.MolFromSmiles(smiles)
            images.append(MolToImage(mol))

        return images