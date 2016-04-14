#include <iostream>
#include <stdio.h>
#include "intel/cpucounters.h"

#define F 200

#define C 200


#define WIDTH 1e3
#define LEVEL 1e5

using namespace std;
using std::vector;


vector<uint8_t> init_arr(uint64_t width) {
  vector<uint8_t> arr;
  arr.resize(width);
  for (int i = 0; i< width; i++) {
    arr[i] = (uint8_t)rand()&1;
  }
  return arr;

}

uint8_t gate_inputs(uint8_t gate) {
  switch (gate) {
    case 0x00: // not
    return 0x01;
    break;
    case 0x01: //and2
    case 0x02: //or2
    case 0x03: // xor2
    case 0x04: //nand2
    case 0x05: //nor2
    case 0x06: //xnor2
    return 0x02;
    break;
    case 0x07: //and3
    case 0x08: //or3
    case 0x09: //nand3
    case 0x0a: //nor3
    case 0x0b: //xor3
    case 0x0c: //xnor3
    case 0x0d: //mux3
    return 3;
    break;
    case 0x0e: // COPY
    case 0x0f:
    return 0;
    break;
  }
}


uint8_t generate_gate() {
  while (1) {
    uint8_t gate = (rand()&1 << 0 |
    rand()&1 << 1 |
    rand()&1 << 2 |
    rand()&1 << 3
  ) & (0xff >> 4);
  if (gate_inputs(gate)) return gate;
}
}

uint64_t generate_stride(uint64_t limit) {
  srand(std::time(0));

  uint64_t currInput = 0;
  uint64_t stride = 0;
  while( stride == 0 ) {
    stride = rand() % limit;
  }
  currInput += stride;
  if( currInput >= limit ) {
    currInput -= limit;
  }
  return currInput;
}



class Gate {
private:
  uint8_t type;
  uint64_t input_1, input_2, input_3;
public:
  Gate();
  Gate(uint8_t, uint64_t, uint64_t, uint64_t);
  uint8_t evaluate(vector<uint8_t> inputs);
};

Gate::Gate(uint8_t t, uint64_t i1, uint64_t i2, uint64_t i3) {
  type = t;
  input_1 = i1; // reference in input array
  input_2 = i2;
  input_3 = i3;
}

Gate::Gate() {
  type = 0;
  input_1 = -1;
  input_2 = -1;
  input_3 = -1;
}

uint8_t Gate::evaluate(vector<uint8_t> inputs) {
  switch (type) {
    case 0x00: // not
    return !inputs[input_1];
    case 0x01: //and2
    return inputs[input_1] & inputs[input_2];
    case 0x02: //or2
    return inputs[input_1] | inputs[input_2];
    case 0x03: //xor2
    return inputs[input_1] ^ inputs[input_2];
    case 0x04: //nand2
    return !(inputs[input_1] & inputs[input_2]);
    case 0x05: //nor2
    return !(inputs[input_1] | inputs[input_2]);
    case 0x06: //xnor2
    return !(inputs[input_1] ^ inputs[input_2]);
    case 0x07: //and3
    return inputs[input_1] & input_2 & inputs[input_3];
    case 0x08: //or3
    return inputs[input_1] | input_2 | inputs[input_3];
    case 0x09: //nand3
    return !(inputs[input_1] & input_2 & inputs[input_3]);
    case 0x0a: //nor3
    return !(inputs[input_1] | input_2 | inputs[input_3]);
    case 0x0b: //xor3
    return inputs[input_1] ^ input_2 ^ inputs[input_3];
    case 0x0c: //xnor3
    return !(inputs[input_1] ^ inputs[input_2] ^ inputs[input_3]);
    case 0x0d: //mux3
    if (inputs[input_1]) return inputs[input_2]; else return inputs[input_3];
    case 0x0e: //copy
    case 0x0f:
    printf("undefined\n");
    exit(0);
    break;
  }
}

class Circuit {
  private:
    vector<vector<Gate>> gates;
  public:
    Circuit();
    vector<uint8_t> evaluate(vector<uint8_t> inputs);
};

Circuit::Circuit() {

  uint64_t input_2,input_3,input_1;
  gates.resize(LEVEL);
  for (int i = 0; i < LEVEL; ++i) {

    gates[i].resize(WIDTH);
    for (int j = 0; j < WIDTH; ++j) {
      uint8_t gateType = generate_gate();
      switch (gate_inputs(gateType)) {
        case 1:
        input_1 = generate_stride(WIDTH);
        input_2 = -1;
        input_3 = -1;
        break;
        case 2:
        input_1 = generate_stride(WIDTH);
        input_2 = generate_stride(WIDTH);
        input_3 = -1;
        break;
        case 3:
        input_1 = generate_stride(WIDTH);
        input_2 = generate_stride(WIDTH);
        input_3 = generate_stride(WIDTH);
        break;
      }
      Gate gate (gateType,input_1,input_2,input_3);
      gates[i][j] = gate;
    }
  }
}

vector<uint8_t> Circuit::evaluate(vector<uint8_t> inputs) {

  vector<uint8_t> outputs;
  outputs.resize(WIDTH);

  for (int i = 0; i< LEVEL; i++) {
    for (int j = 0; j < WIDTH; j++) {
      outputs[j] = gates[i][j].evaluate(inputs);
    }
    inputs = outputs;
  }

  return outputs;
}


void print_counters(SystemCounterState before_sstate, SystemCounterState after_sstate) {
  cout << getExecUsage(before_sstate, after_sstate) << endl;
  cout << getAverageFrequency(before_sstate, after_sstate) << endl;
  cout << getActiveAverageFrequency(before_sstate, after_sstate) << endl;

  cout  << getL2CacheMisses(before_sstate, after_sstate) << endl;
  cout  << getL2CacheHits(before_sstate, after_sstate) << endl;
  cout  << getL2CacheHitRatio(before_sstate, after_sstate) << endl;

  cout  << getL3CacheMisses(before_sstate, after_sstate) << endl;
  cout  << getL3CacheHits(before_sstate, after_sstate) << endl;
  cout  << getL3CacheHitRatio(before_sstate, after_sstate) << endl;
  cout  << getL3CacheOccupancy(getSystemCounterState())  << endl;

  cout  << getBytesReadFromMC(before_sstate, after_sstate) << endl;
  cout  << getBytesWrittenToMC(before_sstate, after_sstate) << endl;
  cout  << getIORequestBytesFromMC(before_sstate, after_sstate) << endl;

  cout  << getInstructionsRetired(before_sstate, after_sstate) << endl;
  cout  << getCycles(before_sstate, after_sstate) << endl;
  cout  << getIPC(before_sstate, after_sstate) << endl;
}

int main() {
  srand(std::time(0));

  PCM * m = PCM::getInstance();
  PCM::ErrorCode returnResult = m->program();
  if (returnResult != PCM::Success){
    std::cerr << "Intel's PCM couldn't start" << std::endl;
    std::cerr << "Error code: " << returnResult << std::endl;
    exit(1);
  }

  cout << "Initial state" << endl;
  SystemCounterState before_sstate = getSystemCounterState();
  SystemCounterState after_sstate = getSystemCounterState();
  cout << "Done" << endl;

  print_counters(before_sstate, after_sstate);

  cout << "Start Generating" << endl;
  before_sstate = getSystemCounterState();
  Circuit circuit;
  after_sstate = getSystemCounterState();
  cout << "Done" << endl;

  print_counters(before_sstate, after_sstate);

  cout << "Evaluating circuit" << endl;
  before_sstate = getSystemCounterState();
  circuit.evaluate(init_arr(WIDTH));
  after_sstate = getSystemCounterState();
  cout << "Done" << endl;

  print_counters(before_sstate, after_sstate);

  m->cleanup();

}
