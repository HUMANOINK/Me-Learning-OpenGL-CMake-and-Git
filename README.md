# Me-Learning-OpenGL-CMake-and-Git
Me trying to learn OpenGL and graphics programming in general, CMake, and Git using the LearnOpenGL tutorial. My first experience with CMake and Git.

It should just build with CMake, as long as you have working installations of Git and Python that CMake can find, and internet access, during the first time configuring for downloading the dependencies with FetchContent and file(DOWNLOAD). Oh and that installation of Python needs to have the Jinja2 module installed because apparently Glad depends on it to generate OpenGL loaders at build time.  
```
pip install Jinja2
```