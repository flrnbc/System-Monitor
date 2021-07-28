#include "linux_parser.h"
#include "process.h"

Process::Process(int pid) {
  pid_ = pid;
  cmd_ = LinuxParser::Command(pid);
  user_ = LinuxParser::User(pid);
  cpuu_ = LinuxParser::CpuUtilization(pid);
  ram_ = LinuxParser::Ram(pid);
  uptime_ = LinuxParser::UpTime(pid);
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuu_; }

// DONE: Return the command that generated this process
string Process::Command() { return cmd_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const &a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}
