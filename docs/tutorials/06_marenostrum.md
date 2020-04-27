This tutorial contains all the considerations on how to deploy PANDORA and their models in Marenostrum 4. It should be similar in other HPC linux-like machines like Nord3 or CTE-Power.

### Tools

When dealing with PANDORA framework, just 3 tools are needed to interact with Marenostrum.

- **ssh** (recommended: ssh on Linux terminal or MinGW64 on Windows (Git bash)): to connect to Marenostrum 4.
- **sftp** (recommended: GUI FileZilla): to upload/download files to/from Marenostrum 4.
- **Git** (already in MN4): VCS to work with PANDORAs code and their models.
- **docker2singularity**: to translate Docker images into Singularity ones, which is the only containers framework available in MN.

## ssh

To connect to MN4 by means of ssh, just open a Linux terminal and update ssh:

```bash
 $ sudo apt update
 $ sudo apt install ssh
```

Then, type:
```
 $ ssh bsc{userID}@mn1.bsc.es
```

, where {userID} should be something like 21473. It should be provided to you by an email from support@bsc.es once you ask for access.

## sftp

sftp is in the same ssh package you have just updated. Nevertheless, a GUI like FileZilla will make thinks more graphical.

Once downloaded, just configure a new connection. Stating the host (mn1.bsc.es), the user (bsc{userID}) and your password should be enough. Also indicate SFTP protocol and 'Normal' logon type.

## Git

## docker2singularity

In a machine with [Docker installed](https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-18-04), just one command is needed to translate a Docker image into a Singularity one:

```
docker run -v /var/run/docker.sock:/var/run/docker.sock \
-v /tmp/test:/output \
--privileged -t --rm \
quay.io/singularity/docker2singularity \
{DockerHub_image}
```

, where the {DockerHub_image} is something like genajim/pandora.

We hardly recommend a Linux installation to execute this command. Also, if Docker have some problems to be installed, try to install the build-essentials, the dkms and the linux-header-$(uname -r) packages previously.

### Creating scripts

### Submittings jobs

### References

Some useful links and tutorials are:

- [Marenostrum User's guide](https://www.bsc.es/support/MareNostrum4-ug.pdf)
- [docker2singularity](https://github.com/singularityhub/docker2singularity)

Additional links:

- [Singularity containers for HPC](docs/tutorials/ContainersHPC-Singularity.pdf)
- [Life sciences use case (jobs submitting & Singularity)](docs/tutorials/MN4BasiscsLife.pdf)
- [Tracing: EXTRAE, PARAVER, DIMEMEAS, etc.](https://tools.bsc.es/downloads)