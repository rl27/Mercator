# Mercator
A tool for visualizing generative models in 3D hyperbolic space.

<hr>

In Visual Studio, add Mercator/OpenGL/includes to the Include Directories under VC++ Directories in the project properties. Add Mercator/OpenGL/lib to the Library Directories. Then, under Linker/Input/Additional Dependencies, add `glfw3.lib` and `opengl32.lib`.

<hr>

On Linux, install the following packages:
```
sudo apt install libgl1-mesa-dev
sudo apt install libglfw3-dev
sudo apt install libxrandr-dev
sudo apt install libxi-dev
```

Then, to compile `main.cpp`, run the following:
```
g++ -LOpenGL/lib -IOpenGL/includes main.cpp glad.c Shader.cpp Tile.cpp Camera.cpp stb_image.cpp -lglfw -lGL -lm -lX11 -lpthread -lXrandr -lXi -ldl
```

<hr>

Additionally, if using WSL 1, follow the instructions in [this link](https://github.com/microsoft/WSL/issues/2855#issuecomment-358861903) to allow OpenGL to run. Importantly, install [VcXsrv](https://sourceforge.net/projects/vcxsrv/). To run VcXsrv, first run XLaunch, then choose Multiple windows, set Display number to 0, and uncheck Native opengl.

If using WSL 2, add the following to your bashrc file:
```
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
export LIBGL_ALWAYS_INDIRECT=0
```
To run [VcXsrv](https://sourceforge.net/projects/vcxsrv/), first run XLaunch, then choose Multiple windows, set Display number to 0, and uncheck Native opengl. **Be sure to also check "Disable access control"**.

Then, open Windows Defender Firewall with Advanced Security -> Inbound Rules and ensure that VcXsrv is allowed access in both private and public networks. An easy way to do this is by deleting all rules for VcXsrv, then running it again through XLaunch.

<hr>

To run the Flask server (which is used to query the generative model), simply run the following:
```
python runserver.py 5555
```
Which will start a server on port 5555.