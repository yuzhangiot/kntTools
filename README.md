# kntTools

Useful tools for knt format files. Include viewer, reader and writer.

See "main.cpp" in each subdirectory for typical usage.

3rd party API dependency:
 |
 |--knt_reader:
 |   |--Eigen-3.2.5
 |   |--lodepng
 |   |--OpenCV-3.0.0
 |
 |--knt_writer:
 |   |--Eigen-3.2.5
 |   |--lodepng
 |   |--OpenCV-3.0.0
 |	
 |--knt_viewer
     |--Eigen-3.2.5
     |--glad (or any other opengl loader you would like to use)
     |--glfw-3.1.1
     |--lodepng
     |--OpenCV-3.0.0
     
     
compile:

$ mkdir build

$ cmake ../src/

$ make -j8

$ ./kntViewer <filePath>

