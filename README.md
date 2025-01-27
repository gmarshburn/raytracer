# raytracer
October 2023

# Overview
My program contains four main folders: camera, raytracer, shapes, and utils. The files in utils handle setting up the scene data, such as parsing the inputted file and initializing all variables needed to render the scene, such as shape, light, and camera data. The files in raytracer handle the main ray tracing algorithm, both for the initial ray to check for visibility and for subsequent rays for shadows and recursive rays for reflections. The files also handle the lighting of each pixel, using the Phong illumination model to determine the collective illumination of each shape, including texture mapping. The files in shapes handle the calculations for each respective implicit shape so that the ray cast in the raytracer can be used to determine the presence of an intersection point via the equations in each file. Finally, the files in camera contain all of the camera's data, such as its view matrix and compositional vectors, so that these values can be easily accessed in other parts of the program with recomputation.

# Known bugs
There are no known bugs in my program.
