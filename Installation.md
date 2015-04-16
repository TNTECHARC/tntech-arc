# Getting started with the TTU ARC codebase #
_(9/4/13 Daniel Oliver, last modified 9/9/13)_

**Step one: A compatible operating system**

> The codebase is native to Linux, Ubuntu especially, for two reasons: legacy code and ease of development. Ease of development and the following lengthy getting started tutorial may seem contradictory but it gets easier quickly. If you already have Ubuntu or some similar Linux distro then skip to Step two.

> Since I avoid hard drive partitioning unless absolutely necessary I like to use VirtualBox to run Ubuntu inside Windows on my laptop and I suggest it as the wise course. This tutorial http://m.wikihow.com/Install-Ubuntu-on-VirtualBox was much better than the original one that I wrote. There are a few things to do differently from the tutorial:
  * DO install networking and usb support when asked.
  * Make the virtual hard drive at least 16GB or more.
  * I use the latest Ubuntu Long-Term-Support version so download that or a newer version.

**Step two: Install development tools**

> There are a few development tools that need to be installed before actually downloading and running the code, such as Git, GNU C compiler, and GNU Make. Open the terminal (command-line) and enter the following commands one at a time:
```
sudo apt-get update  //These first two commands are mostly the Windows update equivalent.

sudo apt-get upgrade

sudo apt-get install build-essential //Install GNU C compiler, GNU Make

c++ -v //Just to check if install was successful
```

**Step three: Download the codebase**
> All the code is of course, in the Git repository hosted here. So, this is the easiest step:
```
sudo apt-get install git //Pretty self apparent by now I hope

cd ~ //Just to make sure you are in your home directory

git clone https://code.google.com/p/tntech-arc/  //Fixed 9/11/13.

cd tntech-arc //Enter the directory

cd asimov //Take a look around at the bare bones
```
> If you really want to get into a world of pain and power, read the [git documentation](http://git-scm.com/book). I'll include how to "commit" changes at a later time but it is not important at the moment.

**Step four: Install Google Protocol Buffers**

> Protocol Buffers is a nice convenient package for data serialization and binary data and the only mildly difficult to install dependency. Linux program installations can be a living nightmare if not using “apt-get” commands. Luckily, Protobuf install is relatively easy though still rather verbose.

> Download “protobuf-2.5.0.tar.gz” from https://code.google.com/p/protobuf/downloads/list and save it in your home directory. Open the terminal for these commands OR you can just follow the google install guide included in the README.txt included in the download.
```
tar -xvf protobuf-2.5.0.tar.gz  //Unzip the archive

cd protobuf-2.5.0

./configure --prefix=/usr  //Configure some installation values

sudo make //Compile the binaries

sudo make check //Just to check if Protobuf is working and compiled correctly

sudo make install //Moves the binaries and libraries to appropriate directories
```

**Step five: Testing**

> Every good software system is well tested, unfortunately this Client-Server codebase is not well tested yet. The best we have so far is at least making sure the system is working since the system has not had extensive use yet. Open a terminal to this directory “~/ttuarc/asimov”.
```
make generate_protoc  //Generates the Protobuf files.  If this fails then Step four did not work

make server_bsd_exe //The Server executable

make run_server_bsd   //Start the server.
```
> Use the key command Ctrl-Shift-‘t’ to open a new terminal in the same directory.
```
make client_bsd_exe //A sample client executable

make run_client_bsd  //Starts the sample client and connects to the server.
```

> If the client says it is connected then everything should be working correctly. If you want further confirmation: connect to the server using a second client and enter _“echo some message it doesn’t really matter what”_. If the clients are both connected then the first client will echo the message from the second client.