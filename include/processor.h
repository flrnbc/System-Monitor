#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor(void);
  float Utilization();  // TODO: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
    float cpuu_;
};

#endif
