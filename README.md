# katori

## DISCLAIMER

**The functions provided by `katori` is for EDUCATIONAL PURPOSES ONLY. You are solely responsible for your actions. Creator of `katori`, in no way responsible for any of the actions performed by anyone.**

If you agree with the above statements, you may proceed to use this tool for anything legal.

## INFO
This is **unbuild version** (I'm rewriting a lot of things right now). Yeah, it's should be in another branch and I very sorry about it, but I'm too lazy to fix it right now. It will be fixed very very soon, I promise!

## General info

`katori` is a tool to work with a network. it's both a sniffer and attacks provider (only few features is implemented for now).

Project is under development, so it's far away from expected tool.

## Features

For now, only `sniffing` option is available. You can specify your interface and file for logging and after this go to `monitor` to see sniffing status.

## How to build and run

```sh
$ make && sudo src/main && make clean
```

Of course, I will add `make install` option soon.
Note that you have to run it using **sudo**, cause sniffing requires privileges