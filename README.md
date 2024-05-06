# Surfel-based Mapping using 3D Laser Range Data

Mapping of 3d laser range data from a rotating laser range scanner, e.g., the Velodyne HDL-64E.
For representing the map, we use surfels that enables fast rendering of the map for point-to-plane ICP and loop closure detection.

## Publication

If you use our implementation in your academic work, please cite the corresponding paper:

J. Behley, C. Stachniss. *Efficient Surfel-Based SLAM using 3D Laser Range Data in Urban Environments*, Proc. of Robotics: Science and Systems (RSS), 2018.

The BibTeX entry for the paper is:  
    
	@inproceedings{behley2018rss, 
			author = {Jens Behley and Cyrill Stachniss},
			title  = {Efficient Surfel-Based SLAM using 3D Laser Range Data in Urban Environments},
			booktitle = {Proc.~of Robotics: Science and Systems~(RSS)},
			year = {2018}  
	}


##  Dependencies

* Qt5 >= 5.2.1
* OpenGL >= 3.3
* libEigen >= 3.2

In Ubuntu 22.04/20.04: Installing all dependencies is accomplished by:
```bash
  $  sudo apt-get install -y build-essential cmake libeigen3-dev libboost-all-dev qtbase5-dev libglew-dev
```

## Build

```
  $ mkdir build && cd build
  $ cmake .. -DCMAKE_BUILD_TYPE=Release -DOPENGL_VERSION=430 -DENABLE_NVIDIA_EXT=YES
  $ make -j5
```
  
  Where you have to set `OPENGL_VERSION` to the supported OpenGL core profile version of your system, which you can query as follows:

```bash
$ glxinfo | grep "version"
server glx version string: 1.4
client glx version string: 1.4
GLX version: 1.4
OpenGL core profile version string: 4.3.0 NVIDIA 367.44
OpenGL core profile shading language version string: 4.30 NVIDIA [...]
OpenGL version string: 4.5.0 NVIDIA 367.44
OpenGL shading language version string: 4.50 NVIDIA
```

 Here the line `OpenGL core profile version string: 4.3.0 NVIDIA 367.44` is important and therefore you should use `-DOPENGL_VERSION = 430`. If you are unsure you can also leave it on the default version `330`, which should be supported by all OpenGL-capable devices.

 If you have a NVIDIA device, like a Geforce or Quadro graphics card, you should also activate the NVIDIA extensions using `-DENABLE_NVIDIA_EXT=YES` for info about the current GPU memory usage of the program.
 
 Now the project root directory should contain a `bin` directory containing the visualizer.

## How to run and use it?

All binaries are copied to the `bin` directory of the source folder of the project. Thus,
1. run `visualizer` in the `bin` directory,
2. open a Velodyne directory from the KITTI Visual Odometry Benchmark and select a ".bin" file,
3. start the processing of the scans via the "play button" in the GUI.

In the `config` directory, different configuration files are given, which can be used as reference to set parameters for some experiments with other data. Specifying the right "vertical Field-of-View" (`data_fov_up` and `data_fov_down`) and the right number of scan lines (`data_height`) are the most important parameters.

See also the [project page](http://jbehley.github.io/projects/surfel_mapping/) for configuration files used for the evaluation in the paper.

## License


Copyright 2018 Jens Behley, University of Bonn.

This project is free software made available under the MIT License. For details see the LICENSE file.


