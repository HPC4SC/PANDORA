Cassandra is a tool intented for the visualization of Pandora's results. All this tutorial is written assuming a Docker installation.

### Compilation

To compile Cassandra you just need to run `make` within the Docker image:

```bash
$ cd ~/PANDORA/cassandra
$ make
```

### Environment preparation

## Windows
- Download X Server for Windows: https://sourceforge.net/projects/vcxsrv/files/latest/download.
- Run XLaunch -> Next -> Next -> Check "Disable access control" -> Next -> Finish.

Then run:

```bash
$ winpty docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it -e DISPLAY=[ipconfig->IPv4]:0.0 -v "[Absolute path to PANDORA/ folder]":/root/PANDORA genajim/pandora
```

E.g.:

```bash
$ winpty docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it -e DISPLAY=192.168.20.81:0.0 -v "C:\Users\bscuser\Documents\GitProjects\PANDORA":/root/PANDORA genajim/pandora
```

## Ubuntu

In the host machine:

```bash
$ xhost +local:root
$ sudo docker run -it -v /tmp/.X11-unix:/tmp/.X11-unix -v [Absolute path to PANDORA/ folder]:/root/PANDORA genajim/pandora
```

E.g.:
```bash
$ xhost +local:root
$ sudo docker run -it -v /tmp/.X11-unix:/tmp/.X11-unix -v /home/german/Documents/IoTwins/PANDORA:/root/PANDORA genajim/pandora
```

Once you're done, for security reasons it is recommended to run:

```bash
$ xhost -local:root
```

## Mac

- Get IP: ifconfig en0 | grep inet | awk '$1=="inet" {print $2}'
- Add the IP Address of your Mac to the X11 allowed list: xhost +


```bash
$ docker run -it -e DISPLAY=[ipconfig->IPv4]:0.0 -v "[Absolute path to PANDORA/ folder]":/root/PANDORA genajim/pandora
```

E.g.:

```bash
$ docker run -it -e DISPLAY=192.168.20.81:0.0 -v "C:\Users\bscuser\Documents\GitProjects\PANDORA":/root/PANDORA genajim/pandora
```

### Execution

In order to execute Cassandra, you just need to do:

```bash
$ cd ~/PANDORA/bin/
$ ./cassandra
```

### FAQ

- I got an error "/usr/bin/env: 'python\r': No such file or directory":

```bash
$ apt install dos2unix
$ dos2unix /root/PANDORA/* 
$ dos2unix /opt/pandora/* 
$ dos2unix /root/PANDORA/bin/mpiCode.py
```