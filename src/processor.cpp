#include "processor.h"
#include "linux_parser.h"

Processor::Processor(void) { cpuu_ = Processor::Utilization(); }

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  cpuu_ = LinuxParser::CpuUtilization();
  return cpuu_;
}
