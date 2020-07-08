# Raspberry Pi uWebSockets.js Builder

If you've been looking to use uWebSockets.js on the Raspberry Pi, this is the tool that you have been looking for.
This tool automatically downloads and compiles the latest version of uWebSockets.js. This creates a NodeJS library module for use in your NodeJS project on the Raspberry Pi.

As you may know, uWebSockets.js is not natively supported on the Raspberry Pi. The reason for this is because uWebSockets.js is only compiled for 64bit, and the Raspberry Pi does not have a fully operational 64bit operating system. Although you can technically get a working 64bit Raspberry Pi OS running, the missing 64bit libraries make it practically useless for solving issues such as this one.

This tool will compile uWebSockets.js right inside your working RbPI environment! This means it will be compiled for 32bit ARM when you run this application.

## How To Use

If you want to run it in a single command, go ahead and paste this into the terminal.

1.) Open up Terminal

2.) CD to the directory that you wish to install/run everything `mkdir uWebSockets` `cd uWebSockets`

3.) Run the following command:

``
curl -s https://raw.githubusercontent.com/jmscreation/RBPI.uWebSockets.js/master/install.sh | bash
``

Note: This will simply run the [install.sh](https://github.com/jmscreation/RBPI.uWebSockets.js/blob/master/install.sh) script located in this repository.
This script is a one line solution to downloading and executing the build application. This will download and compile the uWebSockets.js module automatically.

## How To Compile/Run Manually
Although I personally know it is safe, many will argue that running an application online like this poses as a security threat. Here's how you can avoid the security issues, and ultimately you will be able to compile and run the code yourself.

1.) Open up Terminal

2.) CD to the directory that you wish to install/run everything `mkdir uWebSockets` `cd uWebSockets`

3.) Download the code using the following command

``
wget https://raw.githubusercontent.com/jmscreation/RBPI.uWebSockets.js/master/build.c ./build.c
``

* Optionally you may now check the C code before continuing to make sure everything is safe. *

4.) Compile the [build.c](https://github.com/jmscreation/RBPI.uWebSockets.js/blob/master/build.c) file by running the following command

``
gcc -Wall -o "build" "build.c"
``

5.) Run the build application to build the uWebSockets.js module for NodeJS


### If there are any problems, please let me know

I am not affiliated with the uNetworking team. I simply needed to get uWebSockets.js working for the Raspberry Pi, and decided to develope and share this quick and easy way of getting it working on the Raspberry Pi.

[Official uNetworking](https://github.com/uNetworking/)
[uWebSockets.js](https://github.com/uNetworking/uWebSockets.js)
