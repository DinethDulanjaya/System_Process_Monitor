#include <unistd.h>
#include <cstddef>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using LinuxParser::MemoryType;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the sorted system's processes
vector<Process>& System::Processes() {
  // Collect all available Pids numbers in computer's system
  const vector<int> processes_pids = LinuxParser::Pids();

  processes_.clear();
  for (size_t i = 0; i < processes_pids.size(); ++i) {
    processes_.emplace_back(processes_pids.at(i), cpu_.getTotalJiffies());
  }

  // Sort Processes based on each process' CPU utilization
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }