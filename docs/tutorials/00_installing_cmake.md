
This tutorial will explain how to install Pandora framework, using CMAKE

### Requirements
You will need a PC with GNU/Linux installed. You may use the Docker image at genajim/pandora. The tutorial will explain how to install Pandora on Ubuntu, but any other distribution would work (probably with additional effort related to looking for the correct packages and versions)

### Dependences

To compile and install Pandora we need some libraries and programs to do so, this is a summary of what we need. How to install everything step by step is going to be explained afer this part

* GDAL
* HDF5
* CMAKE
* Git and an accout of it
* Libreries:
	
	```
	mpich
	python3 
	python3-dev
	```
	
* Boost packages:
	
	```
	libboost-python-dev 
	libboost-filesystem-dev 
	libboost-system-dev 
	libboost-timer-dev
	```


- First of all, open a console terminal (like Konsole or xterm) and upgrade your system to the last updates

```bash
 $ sudo apt-get update
 $ sudo apt-get upgrate
```

- Compiling and installing GDAL
	1. To do so download and decompress it using the following commands:

	```bash
	$ wget http://download.osgeo.org/gdal/1.10.1/gdal-1.10.1.tar.gz
	$ tar xvfz gdal-1.10.1.tar.gz
	```

	2. After that to configure the library

	```bash
	$ cd gdal-1.10.1/
	$ CXXFLAGS="-fpermissive" ./configure --prefix=/opt/gdal-1.10.1 --with-pcraster=internal --with-png=internal --with-libtiff=internal --with-geotiff=internal --with-jpeg=internal --with-gif=internal --with-netcdf=no --enable-debug
	```

	3. Then we have to compile and install it

	```bash
	$ make
	$ sudo make install
	```

	4. And finally we have to help cmake find the new gdal libraries. Add these lines to the end of the /etc/environment file and reload it:

	```bash
	$ export GDAL_ROOT=/opt/gdal-1.10.1
	$ export LD_LIBRARY_PATH=${GDAL_ROOT}/lib/:${LD_LIBRARY_PATH}
	$ source /etc/environment
	```
	
- Then we have to return to the home directory to install HDF5
You can do that using the following command:

```bash
cd ~/
```

- Compiling and installing HDF5
	1. HDF5 needs the following library to be installed

	```bash
	$ sudo apt-get install mpich
	```
	2. Then download and decompress it using the following commands:

	```bash
	$ wget https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8/hdf5-1.8.19/src/hdf5-1.8.19.tar.gz	
	$ tar xvfz hdf5-1.8.19.tar.gz
	```

	3. After that to configure the library as well

	```bash
	$ cd hdf5-1.8.19/
	$ ./configure --prefix=/opt/hdf5-1.8.19 --enable-fortran --enable-parallel --enable-debug=all	
	```

	4. Then we have to compile and install it

	```bash
	$ make
	$ sudo make install
	```

	5. And finally we have to help cmake find the new hdf5 libraries. Add these lines to the end of the /etc/environment file and reload it::

	```bash
	$ export HDF5_ROOT=/opt/hdf5-1.8.19
	$ export LD_LIBRARY_PATH=${HDF5_ROOT}/lib/:${LD_LIBRARY_PATH}
	$ source /etc/environment
	```

Return to the home directory using the same command we used before

Before compiling Pandora we need to install some more libraries

- Python
	* We use Python 3, the libraries we need are the following ones:

	```bash
	$ sudo apt-get install python3 python3-dev
	```

- Boost Packages
	* You can install the boost packages that we need using the following command:

	```bash
	$ sudo apt-get install libboost-python-dev libboost-filesystem-dev libboost-system-dev libboost-timer-dev
	```

- Compile Pandora
Now that we have all the dependencies installed let's go and compile Pandora itself

	1. The first thing that we need to do is make the directory for her in our home directory

	```bash
	$ mkdir Pandora
	$ cd Pandora
	```
	
	2. Then we have to download the project using git

	```bash
	$ git clone https://github.com/QuimLaz/PANDORA.git (canviar per la master quan fem el merge)
	```

	3. Now we have to get cmake to be able to generate the makefiles

	```bash
	$ sudo apt-get install cmake
	```
	
	4. Then we have to make a build/ folder to build the makefiles & and a install/ folder to later deploy Pandora:

	```bash
	$ mkdir build
	$ mkdir install
	```
	
	5. After that we need to configure, compile and install Pandora. The values in bold for debug and edebug parameters are the ones by default if nothing is specified.

	```bash
	$ cd build
	$ cmake -D debug=[true|**false**] -D edebug=[true|**false**] -D CMAKE_INSTALL_PREFIX=/${PANDORAPATH} ../
	$ make
	$ sudo make install
	```
	
	6. Now we need to export Pandora's libraries. Add these lines to the end of the /etc/environment file and reload it:

	```bash
	$ export PANDORAPATH=/root/pandora/install
	$ export PATH=${PANDORAPATH}/bin:${PATH}
	$ export PYTHONPATH=${PANDORAPATH}/bin:${PYTHONPATH}
	$ export LD_LIBRARY_PATH=${PANDORAPATH}/lib:${LD_LIBRARY_PATH}
	$ source /etc/environment
	```
	
	7. Finally we need to install scons in order to compile the simulations:
	
	```bash
	$ sudo apt-get install scons
	```
	
	
- Compile Cassandra
Now we will start to compile the UI made for analyzing the results of 
PANDORA's simulations.

	 1. First we need to install Cassandra's dependences:
	 
	 ```bash
	 $ sudo apt-get install libtinyxml-dev libdevil-dev freeglut3-dev libqwt-dev libqt4-dev libqt4-opengl-dev libgdal1-dev build-essential libboost-random-dev libboost-test-dev libboost-timer-dev libboost-chrono-dev
	 ```

	2. Now we just have to compile Cassandra
	
	```bahs
	$ cd ../cassandra/
	$ sudo make
	```
	
Now the Cassandra binary is going to be generated in the ../bin/ directory
        
[Next - Get Started with pyPandora](01_getting_started_pyPandora.md)
Or [Next - Get Started with C++](02_getting_started_pandora.md)
