---
title: School exercise environment
layout: main
---

The goal of this step is for you to get familiar with the ESC working
environment and check that everything is correctly set up for the
school exercises.

If something doesn't work as expected, please ask one of the
organizers.

When using these computing resources you agree to comply with the "Regulation on the use of INFN computing resources",
available in [Italian](https://www.cnaf.infn.it/wp-content/uploads/2020/03/Disciplinare_2020_IT.pdf) and
[English](https://www.cnaf.infn.it/wp-content/uploads/2020/03/Disciplinare_2020_EN.pdf). Please read also the "General
information note on processing of personal data by INFN", available in
[Italian](https://dpo.infn.it/wp-content/uploads/2019/01/Informativa_generale_INFN_181204.pdf) and
[English](https://dpo.infn.it/wp-content/uploads/2020/07/Informativa_generale_181204_EN.pdf).

## Wi-Fi access

Make sure you are using the INFN-Captive WiFi or eduroam networks and
**not** the CEUB WiFi network. Only the INFN-Captive and eduroam networks
will allow to connect to the ESC computers.

## SSH access to school computers

You have been assigned a personal account, with a username of the form
`studentNM`, where `NM` is a number between 34 and 55. In the following,
when you see `student`, replace it with your personal account. You
should also have received the corresponding password.

To log on the computers prepared for the School, you have to go first through a
gateway, named `bastion.cnaf.infn.it`, with the username and password of
`student`.

```shell
[me@mylaptop ~]$ ssh -XA student@bastion.cnaf.infn.it
student@bastion.cnaf.infn.it's password:
Last login: ...
[student@bastion ~]$
```

The `-X` option forwards the X11 display. The `-A` option forwards the SSH agent.

From `bastion` you can then log onto the School computers. There are three
servers available: `hpc-201-11-40.cr.cnaf.infn.it`,
`hpc-200-06-06.cr.cnaf.infn.it`, `hpc-200-06-17.cr.cnaf.infn.it` and `hpc-200-06-18.cr.cnaf.infn.it`. The first
one is the most powerful and should easily support all the students during their
exercises, so prefer that one, in particular for the GPU exercises; the others are meant mainly for the MPI
exercises but are good enough also for all the rest (except GPU hand-on). In fact when doing
exercises that ask to measure performance, feel free to try all the machines and
see if there are differences.

The names of the machines are awful, we know, but you can get around it by
creating a config file for ssh on the `bastion` host, for example:

```shell
[student@bastion ~]$ cat .ssh/config
Host esc
  Hostname hpc-201-11-40.cr.cnaf.infn.it

Host *
  ForwardX11 yes
  ForwardAgent yes

[student@bastion ~]$ ssh esc
student@hpc-201-11-40.cr.cnaf.infn.it's password:
Last login: ...
[student@hpc-201-11-40 ~]$
```

In the following, when you see `esc`, it means one of the servers.

To further simplify the login from `bastion` to `esc`, you may create an SSH key
on `bastion` and register it on `esc`:

```shell
student@bastion ~]$ ssh-keygen
Generating public/private rsa key pair.
Enter file in which to save the key (/home/HPC/student/.ssh/id_rsa):
Enter passphrase (empty for no passphrase):
Enter same passphrase again:
...
student@bastion ~]$ ssh-copy-id esc
...
[student@bastion ~]$ ssh esc
Last login: ...
[student@esc ~]$
```

You don't need to generate different keys for the three servers because they
share the home directories.

See [below](#editing-remotely) for additional SSH configuration.

Your shell is [`bash`](https://www.gnu.org/s/bash).

Please note that:

* The `bastion` host is useful and needed only as a gateway to `esc`. Don't do
  anything else on it.
* Each `esc` computer is not assigned exclusively to a student.
* Your home directory is shared between all `esc` computers, but not with `bastion`.
* Your home directory will be wiped out and your `student` account will be
  disabled soon after the end of the School. Please make sure to save somewhere
  else what you want to keep.

## School material

All the school hands-on material is included in a git repository. Get it using:

```shell
[student@esc ~]$ git clone https://github.com/infn-esc/esc21.git esc
```

The repository contains also these pages.

## Testing the environment

1. Log into `esc`.

1. Enable the right toolsets

```shell
[student@esc ~]$ scl enable devtoolset-8 llvm-toolset-7 bash
```

1. Check the following commands and the respective outputs.

```shell
[student@esc ~]$ gcc --version
gcc (GCC) 8.3.1 20190311 (Red Hat 8.3.1-3)
...
[student@esc ~]$ clang --version
clang version 5.0.1 (tags/RELEASE_501/final)
...
[student@esc ~]$ valgrind --version
valgrind-3.14.0
[student@esc ~]$ perf --version
perf version 3.10.0-957.5.1.el7.x86_64.debug
```

1. Enable the use of gcc 9.2 and check that it's available

```shell
[student@esc ~]$ module load compilers/gcc-9.2.0_sl7
[student@esc ~]$ gcc --version
gcc (GCC) 9.2.0
...
```

1. Enable the use of Threading Building Blocks (TBB)

```shell
[student@esc ~]$ source /shared/software/tbb2019_20191006oss/bin/tbbvars.sh intel64 linux auto_tbbroot
[student@esc ~]$ echo $TBBROOT
/gpfs/gpfs/gpfs_maestro_software_new/tbb2019_20191006oss/bin/..```

## Editing source code

### Editing locally

On `esc` you can find several editors available, such as `vim`, `emacs`, `nano`. If
the X display is available, graphical editors will open a window on your laptop;
the network latency however may not be good enough to give you a fine
experience. Just try.

### Editing remotely

Alternatively you could edit the source code for the exercises on your laptop.

The best option is to use [Visual Studio Code](https://code.visualstudio.com/) with its [remote development
extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.vscode-remote-extensionpack), but you
can also simply synchronize the files with the `esc` machine, for example using one of the following:

* `scp`: You can copy files remotely using `scp`, in both directions.

* `sshfs`: You can mount your `esc` home directory on your laptop via `sshfs`.

* `rsync`: You can synchronize your local workspace with the one you keep on `esc`
  using [`rsync`](http://rsync.samba.org/). Please refer to the `rsync` manual to
  fully understand the meaning of the different options, so to avoid mistakes
  that could cause loss of data.

For all these options to work seamlessly, however, you should first simplify your use of SSH to connect to `esc`,
namely:

* Enable the use of SSH keys in place of password authentication also from your
  laptop to `bastion`. You can reuse the keys you have already generate

* Enable an SSH tunnel on `bastion` in order to be able to connect directly from
  your laptop to `esc`

In practice, adding something like the following to the `~/.ssh/config` file on your laptop should suffice:

```shell
Host bastion
  HostName bastion.cnaf.infn.it

Host esc
  HostName hpc-201-11-40.cr.cnaf.infn.it
  User student
  ProxyJump bastion

Host *
  ForwardX11 yes
  ForwardAgent yes
```
