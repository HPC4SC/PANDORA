
This tutorial will explain how to install Pandora framework, using CMAKE

### Requirements
You will need a PC with GNU/Linux installed. The tutorial will explain how to install Pandora on Ubunt, but any other distribution would work (probably with additional effort related to looking for the correct packages and versions)

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

	4. And finally we have to help cmake find the new gdal libraries

	```bash
	$ export GDAL_ROOT=/opt/gdal-1.10.1
	$ export LD_LIBRARY_PATH=${GDAL_ROOT}/lib/:${LD_LIBRARY_PATH}
	$ echo ""  >> ~/.bashrc
	$ echo "export GDAL_ROOT=/opt/gdal-1.10.1" >> ~/.bashrc	
	$ echo "export LD_LIBRARY_PATH=\${GDAL_ROOT}/lib/:\${LD_LIBRARY_PATH}" >> ~/.bashrc
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

	5. And finally we have to help cmake find the new hdf5 libraries

	```bash
	$ export HDF5_ROOT=/opt/hdf5-1.8.19/
	$ export LD_LIBRARY_PATH=${HDF5_ROOT}/lib/:${LD_LIBRARY_PATH}
	$ echo ""  >> ~/.bashrc
	$ echo "export HDF5_ROOT=/opt/hdf5-1.8.19/"  >> ~/.bashrc
	$ echo "export LD_LIBRARY_PATH=\${HDF5_ROOT}/lib/:\${LD_LIBRARY_PATH}" >> ~/.bashrc
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
Now that we have all the dependencies installed let's go and compile Pandora herself

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
	
	4. Then we have to make a build folder to build the makefiles

	```bash
	$ mkdir build
	$ cd build
	```
	
	5. After that we need to configure, compile and install Pandora

	```bash
	$ cmake -DCMAKE_INSTALL_PREFIX=/opt/pandora ../
	$ make
	$ sudo make install
	```
	
	6. And finally export Pandora's libraries

	```bash
	$ export PANDORAPATH=/opt/pandora/
	$ export PATH=${PANDORAPATH}/bin:${PATH}
	$ export PYTHONPATH=${PANDORAPATH}/bin:${PYTHONPATH}
	$ export LD_LIBRARY_PATH=${PANDORAPATH}/lib:${LD_LIBRARY_PATH}
	```

        
[Next - Get Started with pyPandora](01_getting_started_pyPandora.md)
Or [Next - Get Started with C++](02_getting_started_pandora.md)
