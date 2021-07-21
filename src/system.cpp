#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System(void) {
    kernel_ = LinuxParser::Kernel();
    os_ = LinuxParser::OperatingSystem();
    processes_ = System::Processes();
    cpu_ = System::Cpu();
}


// DONE: Return the system's CPU
Processor& System::Cpu() {
    cpu_ = Processor::Processor();
    return cpu_;
}

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    vector<int> pids = LinuxParser::Pids();
    vector<Process>& processes;

    for (int pid : pids) {
        processes.push_back(Process::Process(pid));
    }

    processes_ = std::sort(processes.begin(), processes.end());

    return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
    return kernel_;
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization;
}

// DONE: Return the operating system name
std::string System::OperatingSystem() {
    return os_;
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// DONE: Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// DONE: Return the number of seconds since the system started running
long int System::UpTime() {
    return LinuxParser::UpTime();
}
