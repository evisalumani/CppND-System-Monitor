#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip> // for setprecision

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, version_literal, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version_literal >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  // See: https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
  const string memTotalKey = "MemTotal:";
  const string memFreeKey = "MemFree:";
  float memTotalValue, memFreeValue;

  string line;
  string key;
  float value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == memTotalKey) {
          memTotalValue = value;
        } else if (key == memFreeKey) {
          memFreeValue = value;
        } 
      }
    }
  }
  return (memTotalValue - memFreeValue)/memTotalValue; // As fraction; it gets multiplied by 100 in NCursesDisplay::ProgressBar
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long upTime; // value in seconds

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }

  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization 
// Utilization in a given instance of time
vector<long> LinuxParser::CpuUtilization() { 
  string line;
  string cpu;
  vector<long> v(10); // We get 10 values for the cpu utilization from /proc/stat

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    // Read only 1st line, for the aggregated CPU information 
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> v[CPUStates::kUser_] >> v[CPUStates::kNice_] >> v[CPUStates::kSystem_] >> v[CPUStates::kIdle_] >> v[CPUStates::kIOwait_] >> v[CPUStates::kIRQ_] >> v[CPUStates::kSoftIRQ_] >> v[CPUStates::kSteal_] >> v[CPUStates::kGuest_] >> v[CPUStates::kGuestNice_];
  }

  return v; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  const string processesKey = "processes";
  string line;
  string key;
  int value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == processesKey) {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  const string processesRunningKey = "procs_running";
  string line;
  string key;
  int value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == processesRunningKey) {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::CpuUtilization(int pid) { 
  string line;
  string value;

  int uTimeIndex = 14, sTimeIndex = 15, cuTimeIndex = 16, csTimeIndex = 17, startTimeIndex = 22;
  long uTime, sTime, cuTime, csTime, startTime;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= startTimeIndex; i++) {
        linestream >> value;
        if (i == uTimeIndex) {
          uTime = std::stol(value);
        } else if (i == sTimeIndex) {
          sTime = std::stol(value);
        } else if (i == cuTimeIndex) {
          cuTime = std::stol(value);
        } else if (i == csTimeIndex) {
          csTime = std::stol(value);
        } else if (i == startTimeIndex) {
          startTime = std::stol(value);
        }
      }
    }
  }

  float totalTime = uTime + sTime + cuTime + csTime; // Also include children processes
  long secondsSinceProcessStarted = LinuxParser::UpTime() - startTime/sysconf(_SC_CLK_TCK);

  // This can be our monitor system, which we just started
  // Avoid division by 0, just return 0% CPU utilization
  if (secondsSinceProcessStarted == 0) {
    return 0;
  }

  float cpuUtilization = (totalTime/sysconf(_SC_CLK_TCK))/secondsSinceProcessStarted; // As fraction; it gets multiplied by 100 in NCursesDisplay::DisplayProcesses
  return cpuUtilization;
}

string LinuxParser::Ram(int pid) {
  const string vmSizeKey = "VmSize:";
  string line;
  string key;
  string value;

  long vmSizeValue;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == vmSizeKey) {
          vmSizeValue = std::stol(value); //  Value is in kB
          break;
        }
      }
    }
  }

  float vmSizeValueInMb = LinuxParser::ConvertKbToMb(vmSizeValue);
  
  // See: https://stackoverflow.com/questions/29200635/convert-float-to-string-with-precision-number-of-decimal-digits-specified
  std::stringstream sstream;
  sstream << std::fixed << std::setprecision(2) << vmSizeValueInMb;
  return sstream.str(); // Return value in MB
}

float LinuxParser::ConvertKbToMb(long kb) {
  float const mbInKb = 0.001;
  return kb*mbInKb;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  const string uidKey = "Uid:";
  string line;
  string key;
  string value;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == uidKey) {
          return value;
        }
      }
    }
  }

  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string key;
  string value;
  string password;

  string uid = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      // Line format is username:password:uid:guid:uid_info:home_dir:cmd_shell
      // Substitute : with space in order to extract the string stream
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> password >> value) {
        if (value == uid) {
          return key;
        }
      }
    }
  }
  return key;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  int startTimeIndex = 22;
  string line;
  string value;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= startTimeIndex; i++) {
        linestream >> value;
      }
    }
  }
  long startTime = std::stol(value); // Expressed in clock ticks
  long startTimeSec = startTime/sysconf(_SC_CLK_TCK); 
  return startTimeSec; 
}