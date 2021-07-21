#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor(void);
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    float cpuu_;
};

#endif
