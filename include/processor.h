#include <string>

#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  Processor(std::string name);
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  int sleep_interval_sec = 1;
  std::string name_;
};

#endif