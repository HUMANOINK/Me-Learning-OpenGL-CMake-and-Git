# Why This is Such a Mess
I'm using this project to learn OpenGL and general graphics programming, CMake and Git, loosely following [Learn OpenGL](https://learnopengl.com/) for the OpenGL and graphics programming part, the main focus.
As this is my first time touching CMake and Git, and second time touching OpenGL and graphics programming, I didn't start out writing perfect CMake, or even just using Git - in part because the learning Git part wan't even decided at that point.
Every time I deleted code I'd written to render something different for the next OpenGL thing to learn, I saved a snapshot of my project locally.

However, polishing-wise, these were in absolutely terrible condition. A quick rundown of differences would be:
* Pre-generated glad files were used, rather than using glad to generate them at build time.
* Every dependency had to be put as part of the source - no use of file(DOWNLOAD) or FetchContent
* Relative paths used to refer to the shader assumed the executable was built into a direct subdirectory of the project directory.
* Certain bad practices that I'm not sure how they occurred. Case-in-point: The header CompileTimeConstants.hpp needed glm/mat4x4.hpp to be included before it but didn't do so itself.

When I finally started using GitHub, I took an hour or two to manually make a lot of changes to each snapshot before committing them.
Of course, I forgot a few. So that's why certain commits seem to randomly introduce a bad practice for no reason. Also explains why the first four demos were committed so close to each-other temporally.
That's also why I made a branch for each "demo". I will almost certainly find things to polish later and will not let go of the past and only apply the changes to the most recent stuff.