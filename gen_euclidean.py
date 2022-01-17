import numpy as np
import numpy.random as npr
import numpy.linalg as npla
import matplotlib.pyplot as plt
import torch
import torchvision

from scipy.spatial.distance import pdist, squareform
from hubconf import PGAN

# git clone https://github.com/facebookresearch/pytorch_GAN_zoo
# cd pytorch_GAN_zoo

npr.seed(8)

ncols  = 8
nrows  = 8
lscale = 5.0 # lengthscale is the main parameter here to look at

# Create a grid of points.
mg_xx, mg_yy = np.meshgrid(np.arange(ncols), np.arange(nrows))
X = np.vstack([mg_xx.ravel(), mg_yy.ravel()]).T

# Get all the pairwise squared distances.
dists = squareform(pdist(X, 'sqeuclidean'))
K = np.exp(-0.5*dists / lscale**2) + 1e-6*np.eye(ncols*nrows)
cK = npla.cholesky(K) # https://en.wikipedia.org/wiki/Multivariate_normal_distribution#Drawing_values_from_the_distribution

noise = cK @ npr.randn(nrows*ncols, 512)

model = PGAN(True, model_name='celebAHQ-512')

print('generating')
with torch.no_grad():
  generated_images = model.test(torch.tensor(noise.astype(np.float32)))
print('done generating')


grid = torchvision.utils.make_grid(generated_images.clamp(min=-1, max=1), nrow=nrows, scale_each=True, normalize=True)

plt.imshow(grid.permute(1, 2, 0).cpu().numpy())
plt.show()
