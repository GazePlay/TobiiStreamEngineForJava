# TobiiStreamEngineForJava
This project allows you to use the Tobii Core SDK Stream Engine API in Java language through the Java Native Interface (JNI)

This project have been tested with Tobii 4C eye-trackers.

It is currently working on:
- [X]  Windows (tested on Windows 10)
- [X]  Linux (tested on Ubuntu 18.04)
- [ ]  MacOS (we're working on it.)


## Installation:

### Windows:

You can simply call the _gazePosition_ function from the _Tobii.java_ class.

### Linux: 

First you need to install the Tobii drivers. You can install them by running the _./install_all_ script:

  `cd drivers/`

  `./install_all`

Then, simply call the _gazePosition_ function from the _Tobii.java_ class.

### MacOS:

Work in progress...
