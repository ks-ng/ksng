# KS:NG
Cybersecurity and penetration testing suite

## How to use

Only works on Linux. To compile it for your system, `git pull` this repository, then go into the repository and `bash ksng/cmplmain.bash`, which will automatically compile the main file with all necessary flags, libraries, etc. You can go into `cmplmain.bash` and verify that you're not being hacked, too - just a G++ command and nothing more.

Once the main file is compiled for your system, you can run the `./kingslayer` executable to load the shell. Again, you can check the main file (`ksng/main.cpp`) to verify it's not a virus. Then, you can use `help` to look at commands and their syntaxes. Have fun. (Note: you may need to run as root or with `sudo` to access network features, since the network interface card reading algorithms require using raw sockets which require sudo)

## What it is

This is a cybersecurity, cryptography, and penetration testing suite that is capable of performing various tests and cryptographic operations as well as viewing network traffic, testing website availability, building custom DNS tables, etc. There are two main parts:

 - the shell, which allows various commands to be run to perform operations and
 - the headers, which you can use to build your own C++ programs.

The suite includes some algorithms like VOX that haven't yet been tested by NIST or anything, so be careful using those. Additionally, be careful what you do with *any* pentesting suite, especially this one - it can be illegal if you don't know what you're doing.

This suite contains raw packet-level network manipulation as well as simple things like TCP connections that you can use to send information to and from devices, as well as network monitoring, encryption and decryption, and a testing system that allows you to create "test files". The command is `runtest` or `rt`. If you place a C++ file in the `ksng/tsts/` directory with the filename `tst_<fn>.cpp`, then the command `rt <fn>` will compile the test file with SSL headers, run the resulting executable, and then clean up after itself. Very useful tool for those that like to see if they're doing something right before moving on.
