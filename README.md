# System Monitor

This is a simple tool to show running processes on a Linux machine. It is based on the [starter code](https://github.com/udacity/CppND-System-Monitor) of a project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213). The code in our repository has been reviewed by one of Udacity's mentors. The main inspiration of this project is clearly [htop](https://github.com/htop-dev/htop).

## Installation 
The System Monitor depends on [ncurses](https://www.gnu.org/software/ncurses/). If it is not yet installed on your machine, please do so via `sudo apt install libncurses5-dev libncursesw5-dev`.

After that simply clone this repository and compile it via `make build` in the folder `System-Monitor`. For more details on the building process, see [Make](https://www.gnu.org/software/make/).

## Interface
The System Monitor is opened via `./build/monitor` (on a Linux machine). It shows the 
* OS (e.g. Ubuntu)
* kernel
* accumulated CPU usage 
* memory usage
* number of total and running processes
* up time
* a list of all running processes with CPU/memory usage etc. as in the screenshot below

![System Monitor](images/System-Monitor_07282021.png)

## Implementation
...

### ncurses
[ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.







