# katori
## General info
**katori** is a tool to work with a network. it's both a sniffer and attacks provider (only few features is implemented for now).

Project is under development, so it's far away from expected tool.

## Features
For now, only `sniffing` option is available. You can specify your interface and file for logging and after this go to `monitor` to see sniffing status.

## How to build and run
```sh
$ make && sudo src/main && make clean
```
Of course, I will add `make install` option soon.
Note that you have to run it using **sudo**, cause sniffing requires privileges