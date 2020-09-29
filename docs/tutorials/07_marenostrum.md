This tutorial contains all the considerations on how to deploy Pandora and their models in Marenostrum 4. It should be similar in other HPC linux-like machines like Nord3 or CTE-Power.

### Tools

When dealing with Pandora framework, just 3 tools are needed to interact with Marenostrum.

- **ssh** (recommended: ssh on Linux terminal or MinGW64 on Windows (Git bash)): to connect to Marenostrum 4.
- **sftp** (recommended: GUI FileZilla): to upload/download files to/from Marenostrum 4.
- **Git** (already in MN4): VCS to work with Pandora's code and their models.
- **docker2singularity**: to translate Docker images into Singularity ones, which is the only containers framework available in MN4.

## ssh

To connect to MN4 by means of ssh, just open a Linux terminal and update ssh:

```bash
$ sudo apt update
$ sudo apt install ssh
```

Then, type:

```bash
$ ssh bsc{userID}@mn1.bsc.es
```

, where {userID} should be something like 21473. It should be provided to you by an email from support@bsc.es once you ask for access.

## sftp & Git

sftp is in the same ssh package you have just updated. Nevertheless, a GUI like FileZilla will make thinks more graphical.

Once downloaded, just configure a new connection. Stating the host (mn1.bsc.es), the user (bsc{userID}) and your password should be enough. Also indicate SFTP protocol and 'Normal' logon type.

Now, you can upload whatever you need to MN4.


You can directly use Git if you prefer for the code. But consider that only node mn0.bsc.es have an Internet connection to connect with GitHub repositories. You can only access to this node if you are BSC staff. Besides, if you need to access outside of the BSC, you will need to use a [VPN](https://gateway.bsc.es/) to connect to BSC network.

Change between normal login nodes (1-5) to login0 is not allowed. So, you will need to execute `$ ssh bsc{userID}@mn0.bsc.es` from the outside (and then you could do `$ ssh login1`).

```bash
$ cd ~
$ git clone https://github.com/HPC4SC/PANDORA.git
```

To be sure that Pandora directory is at your ~ directory is a must

## docker2singularity

We hardly recommend a Linux installation to translate a Docker image into a Singularity one. Also, if Docker at first instance have some problems to be installed, try to install the build-essentials, the dkms and the linux-header-$(uname -r) packages previously. Then, try to [install Docker ](https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-18-04).

Assuming a machine with Docker installed, just one command is needed to translate a Docker image into a Singularity one:

```bash
$ docker run    -v /var/run/docker.sock:/var/run/docker.sock \
                -v /tmp/test:/output \
                --privileged -t --rm \
                quay.io/singularity/docker2singularity \
                {DockerHub_image}
```

, where the {DockerHub_image} should be something like genajim/pandora.

The resulting image has been saved in /tmp/test/. Now you can upload it to MN4 by sftp.

### Working with Singularity - Pandora & models compilation

Singularity is already installed in MN4. In order to compile Pandora and its models, you need to use Singularity:

```bash
$ module purge
$ module load singularity
$ singularity shell genajim_pandora.sif
```

Now, check that the environment variables for the deployment are correct. If not, you should add the following lines in the ~/.bashrc file:

```bash
export LD_LIBRARY_PATH=""

export PANDORAPATH=~/PANDORA/install
export LD_LIBRARY_PATH=${PANDORAPATH}/lib/:${LD_LIBRARY_PATH}

export PATH=${PANDORAPATH}/bin:${PATH}
export PYTHONPATH=${PANDORAPATH}/bin:${PYTHONPATH}

export GDAL_ROOT=/opt/gdal-1.10.1
export LD_LIBRARY_PATH=${GDAL_ROOT}/lib/:${LD_LIBRARY_PATH}

export HDF5_ROOT=/opt/hdf5-1.8.19
export LD_LIBRARY_PATH=${HDF5_ROOT}/lib/:${LD_LIBRARY_PATH}
```

And then load ~/.bashrc:

```bash
$ source ~/.bashrc
```

**IMPORTANT!** Consider that the ~/.bashrc file is shared between the Singularity image and your MN profile.

From here on, the compilation steps are exactly the same ones than those used when compiling in local ([Compile and Install (using CMAKE)](00_installing_cmake.md)). You can use the scripts at PANDORA/scripts/compileAndExecScripts/, which allow you to rapidly clean everything, deploy and run any model at examples/.

### Creating scripts

Once in MN4 and having created the Singularity image with the previous command, create a folder MNBatchOutput and open a new file called batchScript.cmd (for instance with vim). Then write the following single core template:

```bash
#!/bin/bash

#SBATCH --job-name=sugarScapeBatchTest
#SBATCH --workdir=.
#SBATCH --ntasks=1
#SBATCH --time=00:01:00
#SBATCH --error=./MNBatchOutput/%j-{modelName}.err
#SBATCH --output=./MNBatchOutput/%j-{modelName}.out

module purge                    # This will remove all the previously loaded packages
module load singularity         # This will load the latest installed version of Singularity in MN4

singularity exec --pwd ~/PANDORA/examples/modelDirectory ~/{singularityImage} ./{modelName}
```

For instance, for the sugarScape model, the script should be something like:

```bash
#!/bin/bash

#SBATCH --job-name=sugarScapeBatchTest
#SBATCH --workdir=.
#SBATCH --ntasks=1
#SBATCH --time=00:01:00
#SBATCH --error=./MNBatchOutput/%j-sugarScape.err
#SBATCH --output=./MNBatchOutput/%j-sugarScape.out

module purge                    # This will remove all the previously loaded packages
module load singularity/3.5.2   # This will load the latest installed version of Singularity in MN4

singularity exec --pwd ~/PANDORA/examples/sugarScape ~/genajim_pandora.sif ./sugarScape
```

Adjust the *--time* parameter as the maximum time you think your app is going to last.

For a multicore (hybrid MPI + OpenMP) setup, the following parameters should be added/changed:

```bash
#!/bin/bash

#SBATCH --job-name=sugarScapeBatchTest
#SBATCH --workdir=.
#SBATCH --ntasks=12
#SBATCH --cpus-per-task=4
#SBATCH --tasks-per-node=6
#SBATCH --time=00:01:00
#SBATCH --error=./MNBatchOutput/%j-sugarScape.err
#SBATCH --output=./MNBatchOutput/%j-sugarScape.out

module purge                    # This will remove all the previously loaded packages
module load singularity/3.5.2   # This will load the latest installed version of Singularity in MN4
module load impi/2017.4         # Intel MPI implementation

mpirun singularity exec --pwd ~/PANDORA/examples/sugarScape ~/pandora_ext_singularity.sif ./sugarScape
```

This specific example is showing how to request 2 nodes, each one with 6 tasks, and assigning 4 cpus for each task. In short: there will be 12 MPI processes in total, and 4 cores for each one of these processes executing threads by means of OpenMP (if implemented). It would make sense to just indicate the *--ntasks*, letting the system to try to allocate whole nodes¿?

Due to the fact that MN counts with nodes with 48 cores, any 2 nodes with 24 free cores (6 tasks * 4 cpus/task) each one of them, will be assigned to your task. In the other hand, we could request for full nodes with the following directive only:

```bash
#SBATCH --nodes=4
```

As we are requesting here for full nodes, it will take more time for the job to be allocated in that resources and be executed. In other words, it will spend more time in the queue. For this reason, it is recommended to use the parameters above, instead of this last one. A useful configuration would be something like:

```bash
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=48
```

In which 8 MPI processes will be created, each one of them containing 48 cpus/threads for the OpenMP part. Since each node of MN4 is composed by exactly 48 cores, you will fully occupy 8 nodes. If you consider that 48 threads is too much for the amount of work you have in each node (partition in Pandora), just decrease the `--cpus-per-task` parameter. The number of MPI processes will remain 8, but maybe it take less to allocate that resources since it is a more flexible configuration (you don't need full nodes but only a portion of each). You should play with these 2 parameters in order to find the best configuration for a model.

### Submittings jobs

You can use the `sbatch <job_script>` directive to submit jobs:

```bash
$ sbatch sugarScapeScript.cmd
$ watch -n 1 squeue
```

To cancel a job just use `scancel <job_id>` directive.

### References

Some useful links and tutorials are:

- [Marenostrum User's guide](https://www.bsc.es/support/MareNostrum4-ug.pdf)
- [docker2singularity](https://github.com/singularityhub/docker2singularity)

Additional links:

- [Singularity containers for HPC](../ContainersHPC-Singularity.pdf)
- [Life sciences use case (jobs submitting & Singularity)](../MN4BasiscsLife.pdf)
- [Tracing: EXTRAE, PARAVER, DIMEMEAS, etc.](https://tools.bsc.es/downloads)