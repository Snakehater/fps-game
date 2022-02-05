Create a new folder called opengl_dev and change the current directory to the new path:

mkdir ~/opengl_dev
cd ~/opengl_dev

Copy
Obtain a copy of the GLFW source package (glfw-3.0.4) from the official repository: http://sourceforge.net/projects/glfw/files/glfw/3.0.4/glfw-3.0.4.tar.gz.

Extract the package.

tar xzvf glfw-3.0.4.tar.gz

Copy
Perform the compilation and installation:

cd glfw-3.0.4
mkdir build
cd build
cmake ../
make && sudo make install
