#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
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
  string key, unit, line;
  float value;
  vector<float> mem_data{};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    for (int i = 0; i < 5; i++){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
      mem_data.push_back(value);
    }
  }
  float utilization = (mem_data.at(MemoryType::kMemTotal_) - mem_data.at(MemoryType::kMemFree_) - mem_data.at(MemoryType::kMemBuffer_) - mem_data.at(MemoryType::kMemCached_)) / mem_data.at(MemoryType::kMemTotal_);
  return utilization;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
	string line;
  long value{0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    return value;
  }
  return value;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
 string key, line;
  long int value{0}, active_jiffies{0};
  std::ifstream filestream (kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
   std::getline (filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> key;
    }
    for (int i = 0; i < 4; i++) {
      linestream >> value;
      active_jiffies += value;
    }
    return active_jiffies;
  }
  return active_jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line, cpu_string;
  long value, active_jiffies{0};
  vector<int> cpu_data{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
   std::getline (filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu_string;
    while (linestream >> value) {
     cpu_data.push_back(value); 
    }
  }
  active_jiffies = cpu_data.at(CPUStates::kUser_) + cpu_data.at(CPUStates::kNice_) + cpu_data.at(CPUStates::kSystem_) + cpu_data.at(CPUStates::kIRQ_) + cpu_data.at(CPUStates::kSoftIRQ_) + cpu_data.at(CPUStates::kSteal_);
  
  return active_jiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
    string line, cpu_string;
  long value, idle_jiffies{0};
  vector<int> cpu_data{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
   std::getline (filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu_string;
    while (linestream >> value) {
     cpu_data.push_back(value); 
    }
  }
  idle_jiffies = cpu_data.at(CPUStates::kIdle_) + cpu_data.at(CPUStates::kIOwait_);
  
  return idle_jiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, cpu, value;
  vector<string> jiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
   std::getline (filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
     jiffies.push_back(value); 
    }
  }
  return jiffies;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int processes;
  string key, line;
  std::ifstream filestream (kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline (filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes"){
       linestream >> processes;
        break;
      }
    }
  }
  return processes;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int processes;
  string key, line;
  std::ifstream filestream (kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
   while (std::getline(filestream, line)) {
    std::istringstream linestream (line);
     linestream >> key;
     if (key == "proc_running") {
      linestream >> processes;
       break;
     }
   }
  }
  return processes;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline (filestream, command);
  }
  return command;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, memStr;
  long mem;
  std::ifstream filestream (kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
   while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
     linestream >> key;
     if (key == "VmSize:") {
      linestream >> mem;
       mem /= 1000;
       memStr = to_string(mem);
       break;
     }
   }
  }
  return memStr;
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, uid;
  std::ifstream filestream (kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline (filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
      break;
      }
    }
  }
  return uid;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string id, x, temp, line;
  string name = "DEFAULT";
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
   while (std::getline (filestream, line)) {
    std::replace (line.begin(), line.end(), ':', ' ');
     std::istringstream linestream(line);
     linestream >>temp >> x >> id;
     if (id == uid) {
       name = temp;
       break;
     }
   }
  }
  return name;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  long starttime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
     values.push_back(value); 
    }
  }
  try {
   starttime = std::stol(values[21]) / sysconf(_SC_CLK_TCK); 
  }catch(...) {
   starttime = 0; 
  }
  return starttime;
}
