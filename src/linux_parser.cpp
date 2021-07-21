#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// helper functions
string GetValue(string file, string wanted_key) {
    string key, value, line;
    std::ifstream filestream(file);

    while (std::getline(filestream, line)) {
        std::istringstream sstream(line);
        sstream >> key >> value;

        if (key == wanted_key) return value;
    }
}

vector<string> GetWordsFromLine(string file, string wanted_key, bool first_line = false) {
    string key, value, line;
    vector<string> words;
    std::ifstream filestream(file);

    if (first_line) {
        std::getline(filestream, line);
        std::istringstream stream(line);

        while (stream >> word) {
            words.push_back(word);
        }
        return words;
    }

    while (std::getline(filestream, line)) {
        std::istringstream sstream(line);
        sstream >> key;

        if (key == wanted_key) break;
    }

    std::istringstream stream(line); // seems to be necessary but why?
    while (stream >> word) {
        words.push_back(word);
    }
    return words;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line); // only one line
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    // format in /proc/version is 'Linux version kernel ...'
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    string file = kProdcDirectory + kMeminfoFilename;
    float MemTotal = std::stof(GetValue(file, "MemTotal:"));
    float MemFree = std::stof(GetValue(file, "MemFree:"));

    return MemTotal - MemFree;
}

// DONE: Read and return the system uptime (in seconds)
long LinuxParser::UpTime() {
    long uptime = std::stol(GetWordsFromLine(kProcDirectory + kUptimeFilename, "", true)[0]);

    return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    return LinuxParser::ActiveJiffies + LinuxParser::IdleJiffies;
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// ActiveJiffies = utime (#14) + stime (#15)? (note shift by -1)
long LinuxParser::ActiveJiffies(int pid) {
    string pid_string = std::to_string(pid); // necessary?
    string filename = kProcDirectory + pid_string + kStatusFilename;
    vector<string> words = GetWordsFromLine(filename, "", true);

    return std::stol(words[13]) + std::stol(words[14]);
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    // TODO: ActiveJiffies = NonIdle?
    // if so: ActiveJiffies = user + nice + system + irq + softirq + steal
    // TODO: need CPUStates?
    vector<string> jiffies_strings = GetWordsFromLine(kProcDirectory + kStatFilename, "cpu");
    int user = std::stoi(jiffies_strings[1]);
    int nice = std::stoi(jiffies_strings[2]);
    int system = std::stoi(jiffies_strings[3]);
    int irq = std::stoi(jiffies_strings[6]);
    int softirq = std::stoi(jiffies_strings[7]);
    int steal = std::stoi(jiffies_strings[8]);

    return user + nice + system + irq + softirq + steal;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    // IdleJiffies = idle + iowait
    vector<string> jiffies_strings = GetWordsFromLine(kProcDirectory + kStatFilename, "cpu");
    int idle = std::stoi(jiffies_strings[4]);
    int iowait = std::stoi(jiffies_strings[5]);

    return idle + iowait;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
    // instead of returning utilization of all CPUs (vector<string>)
    // only the aggreated utilization (-> float)
    return LinuxParser::ActiveJiffies/LinuxParser::Jiffies; // TODO: problem with conversion?
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    return std::stoi(GetValue(kProdDirectory + kStatFilename, "processes"));
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    return std::stoi(GetValue(kProdDirectory + kStatFilename, "procs_running"));
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
    string filename = kProcDirectory + std::to_string(pid) + kStatusFilename;

    return GetValue(filename, "VmSize:")) + " KB"; // TODO: convert to MB?
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
// process_uptime = system_uptime * sysconf(_SC_CLK_TCK) - starttime  (in jiffies)
long LinuxParser::UpTime(int pid) {
    string pid_string = std::to_string(pid);
    string filename = kProcDirectory + pid_string + kStatFilename;
    long  starttime = std::stol(GetWordsFromLine(filename, "", true)[21]); // in jiffies

    return LinuxParser::UpTime() - (starttime / sysconf(_SC_CL_TCK)); // issue with type?
}
