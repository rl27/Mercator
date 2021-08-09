from abc import ABC, abstractmethod
from typing import List
from PIL import Image


class HyperbolicGenerativeModel(ABC):

    @abstractmethod
    def generate_image_from_coords(self, coords: List[float]) -> Image:
        pass
