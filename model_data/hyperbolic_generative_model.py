from abc import ABC, abstractmethod
from PIL import Image


class HyperbolicGenerativeModel(ABC):

    @abstractmethod
    def generate_image_from_latent_vector(self, v) -> Image:
        pass
