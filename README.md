# HyperViz
A tool for visualizing generative models in 3D hyperbolic space.

In Visual Studio, add HyperViz/OpenGL/includes to the Include Directories under VC++ Directories in the project properties. Add HyperViz/OpenGL/lib to the Library Directories. Then, under Linker/Input/Additional Dependencies, add `glfw3.lib`.

On Linux, install the following packages:
```
sudo apt install libgl1-mesa-dev
sudo apt install libglfw3-dev
sudo apt install libxrandr-dev
sudo apt install libxi-dev
```

To compile `main.cpp`, run the following:
```
g++ -LOpenGL/lib -IOpenGL/includes main.cpp glad.c Shader.cpp Tile.cpp Camera.cpp stb_image.cpp -lglfw -lGL -lm -lX11 -lpthread -lXrandr -lXi -ldl
```

Additionally, if using WSL, follow the instructions in [this link](https://github.com/microsoft/WSL/issues/2855#issuecomment-358861903) to allow OpenGL to run.