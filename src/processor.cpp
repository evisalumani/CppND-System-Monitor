#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <unistd.h> // for usleep

Processor::Processor(): name_("cpu") {}
Processor::Processor(std::string name): name_(name) {}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    // Measure CPU utilization in within a short interval
    std::vector<long> previousUtilization = LinuxParser::CpuUtilization();
    usleep(sleep_interval_sec*1000000); 
    std::vector<long> utilization = LinuxParser::CpuUtilization();
    
    // See: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    long previousIdle = previousUtilization[LinuxParser::CPUStates::kIdle_] + previousUtilization[LinuxParser::CPUStates::kIOwait_];
    long idle = utilization[LinuxParser::CPUStates::kIdle_] + utilization[LinuxParser::CPUStates::kIOwait_];

    long previousNonIdle = previousUtilization[LinuxParser::CPUStates::kUser_] + previousUtilization[LinuxParser::CPUStates::kNice_] + previousUtilization[LinuxParser::CPUStates::kSystem_] + previousUtilization[LinuxParser::CPUStates::kIRQ_] + previousUtilization[LinuxParser::CPUStates::kSoftIRQ_] + previousUtilization[LinuxParser::CPUStates::kSteal_];
    long nonIdle = utilization[LinuxParser::CPUStates::kUser_] + utilization[LinuxParser::CPUStates::kNice_] + utilization[LinuxParser::CPUStates::kSystem_] + utilization[LinuxParser::CPUStates::kIRQ_] + utilization[LinuxParser::CPUStates::kSoftIRQ_] + utilization[LinuxParser::CPUStates::kSteal_];

    long previousTotal = previousIdle + previousNonIdle;
    long total = idle + nonIdle;

    float totalDiff = total - previousTotal;
    float idleDiff = idle - previousIdle;

    float cpuPercentage = (totalDiff - idleDiff)/totalDiff;
    return cpuPercentage;
}