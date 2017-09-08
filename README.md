# libsx1509

This project demonstrates interfacing of SX1509 with 96Boards CE Platform using libmraa.
SX1509 is a GPIO expander that uses the i2c interface to provide 16 GPIOs all of which are 8bit PWM compatable

# Table of Contents
- [1) Hardware requirements](#1-hardware-requirements)
- [2) Software](#2-software) 
   - [2.1) Operating System](#21-operating-system)
   - [2.2) Package Dependencies](#22-package-dependencies)
- [3) Building and Running](#3-building-and-running)
- [4) Conclusion](#4-conclusion)

# 1. Hardware requirements:

1. [Dragonboard 410c or any of 96Boards CE](http://www.96boards.org/product/dragonboard410c/)
2. [SX1509 Module](https://www.sparkfun.com/products/13601)
3. Connecting cables
4. 4x LEDs
5. Switch

# 2. Software

## 2.1 Operating System

- [Linaro Debian based OS (latest)](https://github.com/96boards/documentation/blob/master/ConsumerEdition/DragonBoard-410c/Downloads/Debian.md)

## 2.2 Package Dependencies

1. MRAA Library
``` shell
$ sudo apt-get install libmraa-dev
```

# 3. Building and Running:

``` shell
$ git clone https://github.com/96boards-projects/libsx1509
$ cd libsx1509
$ make
$ ./example
```

# 4. Conclusion:

The libsx1509 library provides support for the SX1509 GPIO expander and builds on libmraa. It provides functions such as digitalRead digitalWrite and anaglogWrite in a similar fashion to that of Arduino IDE. For detailed info take a look at the example.cpp and libsx1509.h files.
