#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long jiffies = LinuxParser::Jiffies();
  long idle_jiffies = LinuxParser::IdleJiffies();
  
  // calculate utilization
  float jiffies_diff = (float)(jiffies - jiffies_);
  float idle_diff = (float)(idle_jiffies - idle_);
  
  // Update private members
  jiffies_ = jiffies;
  idle_ = idle_jiffies;
  
  return (jiffies_diff - idle_diff) / jiffies_diff;
}