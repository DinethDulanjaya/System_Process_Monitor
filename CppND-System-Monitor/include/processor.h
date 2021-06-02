#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>
#include <vector>

using std::vector;
using std::array;

class Processor {
 public:
  Processor();
  float Utilization();  // DONE: See src/processor.cpp
  long getTotalJiffies();

  // DONE: Declare any necessary private members
 private:
  void calculateDeltaTime(array<long, 3>&, array<long, 3>&);
  virtual void getLinuxParserForJiffies(array<long, 3>&);
  float calculateUtilization();
  void updatePrevJiffies();
  
  array<long, 3> current_cpu_jiffies_;
  array<long, 3> prev_cpu_jiffies_;
  array<float, 3> delta_jiffies_;
};

#endif