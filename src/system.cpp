#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() { 
    cpu_ = Processor(); // The aggregate CPU

    vector<int> pids = LinuxParser::Pids();
    for (int pid : pids) {
        processes_.push_back(Process(pid));
    }
    
    memory_utilization_ = LinuxParser::MemoryUtilization();
    up_time_ = LinuxParser::UpTime();
    total_processes_ = LinuxParser::TotalProcesses();
    running_processes_ = LinuxParser::RunningProcesses();
    kernel_ = LinuxParser::Kernel();
    opereating_system_ = LinuxParser::OperatingSystem();
}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    // Return processes sorted in descending order by their CPU utilization
    sort(processes_.begin(), processes_.end(), [](const Process & a, const Process & b) -> bool
    { 
        return b < a; 
    });
    return processes_; 
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return memory_utilization_; }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return opereating_system_; }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return running_processes_; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return total_processes_; }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return up_time_; }