#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// helper functions
string GetValue(string file, string wanted_key) {
    string key, line, value;
    std::ifstream filestream(file);
  

    while (std::getline(filestream, line)) {
        std::istringstream sstream(line);
        sstream >> key >> value;

        if (key == wanted_key) return value;
    }
  return "0";
}

vector<string> GetWordsFromLine(string file, string wanted_key, bool first_line = false) {
    string key, line, word;
    vector<string> words(50,  "0"); // limit is OK because we do not read longer lines
    std::ifstream filestream(file);

    if (filestream.is_open()) {
        if (first_line) {
            std::getline(filestream, line);
            std::istringstream stream(line);
          	int i = 0;

            while (stream >> word && i <= 49) {
                words[i] = word;
              	i += 1;
            }
            return words;
        }

        while (std::getline(filestream, line)) {
            std::istringstream sstream(line);
            sstream >> key;

            if (key == wanted_key) break;
        }

        std::istringstream stream(line); // seems to be necessary but why?
      	int j = 0;
      
        while (stream >> word && j <= 49) {
            words[j] = word;
          	j += 1;
        }
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
    string file = kProcDirectory + kMeminfoFilename;
    float MemTotal = std::stof(GetValue(file, "MemTotal:"));
    float MemFree = std::stof(GetValue(file, "MemFree:"));

    return (MemTotal - MemFree)/MemTotal;
}

// DONE: Read and return the system uptime (in seconds)
long LinuxParser::UpTime() {
    long uptime = std::stol(GetWordsFromLine(kProcDirectory + kUptimeFilename, "", true)[0]);

    return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// ActiveJiffies = utime (#14) + stime (#15)? (note shift by -1)
long LinuxParser::ActiveJiffies(int pid) {
    string pid_string = std::to_string(pid); // necessary?
    string filename = kProcDirectory + pid_string + kStatFilename;
    vector<string> words = GetWordsFromLine(filename, "", true);

    return std::stol(words[13]) + std::stol(words[14]) + std::stol(words[15]) + std::stol(words[16]);
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    // TODO: ActiveJiffies = NonIdle?
    // if so: ActiveJiffies = user + nice + system + irq + softirq + steal
    // TODO: need CPUStates?
    vector<string> jiffies = GetWordsFromLine(kProcDirectory + kStatFilename, "cpu");
    long user = std::stol(jiffies[1]);
    long nice = std::stol(jiffies[2]);
    long system = std::stol(jiffies[3]);
    long irq = std::stol(jiffies[6]);
    long softirq = std::stol(jiffies[7]);
    long steal = std::stol(jiffies[8]);

    return user + nice + system + irq + softirq + steal;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    // IdleJiffies = idle + iowait
    vector<string> idle_jiffies = GetWordsFromLine(kProcDirectory + kStatFilename, "cpu");
    long idle = std::stol(idle_jiffies[4]);
    long iowait = std::stol(idle_jiffies[5]);

    return idle + iowait;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
    // instead of returning utilization of all CPUs (vector<string>)
    // only the aggreated utilization (-> float)
  	long jiffies = LinuxParser::Jiffies();
  	float active_jiffies = 1.0*LinuxParser::ActiveJiffies();
  
  	if (jiffies != 0) {
      return active_jiffies/jiffies; // TODO: problem with conversion?
    }
  
	return 0;
}

float LinuxParser::CpuUtilization(int pid) {
  float active_jiffies = 1.0*LinuxParser::ActiveJiffies(pid);
  long uptime = LinuxParser::UpTime(pid);
	
  if (uptime != 0) {
    return active_jiffies/(uptime*sysconf(_SC_CLK_TCK));
  }  
  
  return 0;
}


// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    return std::stoi(GetValue(kProcDirectory + kStatFilename, "processes"));
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    return std::stoi(GetValue(kProcDirectory + kStatFilename, "procs_running"));
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
    string cmd;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

    if (filestream.is_open()) {
        std::getline(filestream, cmd);
    }
    return cmd;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
    string filename = kProcDirectory + std::to_string(pid) + kStatusFilename;
  	long mem_kb = std::stol(GetValue(filename, "VmSize:")); // TODO: convert to MB?
  	float mem_mb = mem_kb / 1000;
  	long mem_trunc = std::trunc(mem_mb);
  
  return std::to_string(mem_trunc);
  	
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
    string filename = kProcDirectory + std::to_string(pid) + kStatusFilename;

    return GetValue(filename, "Uid:");
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
    string uid = LinuxParser::Uid(pid);
    string user, line, pwd, userID;
    std::ifstream filestream(kPasswordPath);

    if (filestream.is_open()) {
        while(std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            linestream >> user >> pwd >> userID;
            if (userID == uid) break;
        }
    }
    return user;
}

// DONE: Read and return the uptime of a process (in seconds)
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
    string pid_string = std::to_string(pid);
    string filename = kProcDirectory + pid_string + kStatFilename;
    long starttime = std::stol(GetWordsFromLine(filename, "", true)[21]); // in jiffies

    return LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK)); // issue with type?
}
