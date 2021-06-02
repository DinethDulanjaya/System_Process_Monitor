#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using LinuxParser::MemoryType;
using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid, const long total_jiffies) : pid_(pid) {
  calculateCpuUtilization(total_jiffies);
}

void Process::calculateCpuUtilization(const long total_jiffies) {
  const long active_jiffies = LinuxParser::ActiveJiffies(pid_);
  cpu_utilization_ = active_jiffies * 1.0 / total_jiffies;
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { return cpu_utilization_; }

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return to_string(std::stol(LinuxParser::Ram(pid_))); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects sorting
bool Process::operator<(Process const& a) const {
  return (this->cpu_utilization_ > a.cpu_utilization_)? true : false;
}