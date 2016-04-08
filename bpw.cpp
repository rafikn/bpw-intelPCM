#include <iostream>
#include <stdio.h>
#include "intel/cpucounters.h"

#define F 200

#define C 200

using namespace std;


uint8_t * init_arr(uint8_t * arr, uint8_t width) {
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




void simulate() {

  uint64_t input_1;
  uint64_t input_2;
  uint64_t input_3;

  uint64_t width = 1e4;
  uint64_t levels = 1e4;
  uint8_t gateType;

  uint8_t	 * in = (uint8_t *) calloc(width, sizeof(uint8_t));
  uint8_t	 * out = (uint8_t *) calloc(width, sizeof(uint8_t));

  for (int i = 0; i < levels; i++) {
    in = out;
    if (i == 0) {
      in = init_arr(in, width);
    }

    for (int j = 0; j < width; j++) {

      //gateType = read_byte(ptr); ptr++;
      gateType = generate_gate();
      switch (gate_inputs(gateType)) {
        case 1:
          //input_1 = in_arr[read_byte(ptr)]; ptr+=8;
          input_1 = generate_stride(width);
          break;
        case 2:
          //input_1 = in_arr[read_byte(ptr)]; ptr+=8;
          input_1 = generate_stride(width);
          //input_2 = in_arr[read_byte(ptr)]; ptr+=8;
          input_2 = generate_stride(width);
          break;
        case 3:
          //input_1 = in_arr[read_byte(ptr)]; ptr+=8;
          input_1 = generate_stride(width);
          //input_2 = in_arr[read_byte(ptr)]; ptr+=8;
          input_2 = generate_stride(width);
          //input_3 = in_arr[read_byte(ptr)]; ptr+=8;
          input_3 = generate_stride(width);
          break;
      }
    }
  }
}

int main() {


    PCM * m = PCM::getInstance();
    PCM::ErrorCode returnResult = m->program();
    if (returnResult != PCM::Success){
       std::cerr << "Intel's PCM couldn't start" << std::endl;
       std::cerr << "Error code: " << returnResult << std::endl;
       exit(1);
    }

    SystemCounterState before_sstate = getSystemCounterState();

    simulate();

    SystemCounterState after_sstate = getSystemCounterState();

    cout << "EXEC: " << getExecUsage(before_sstate, after_sstate) << endl;
    cout << "IPC: " << getIPC(before_sstate, after_sstate) << endl;
    cout << "FREQ: " << getAverageFrequency(before_sstate, after_sstate) << endl;
    cout << "AFREQ: " << getActiveAverageFrequency(before_sstate, after_sstate) << endl;
    cout << "L3MISS: " << getL3CacheMisses(before_sstate, after_sstate) << endl;
    cout << "L2MISS: " << getL2CacheMisses(before_sstate, after_sstate) << endl;
    cout << "L3HIT: " << getL3CacheHits(before_sstate, after_sstate) << endl;
    cout << "L3HITR: " << getL3CacheHitRatio(before_sstate, after_sstate) << endl;
    cout << "L2HIT: " << getL2CacheHits(before_sstate, after_sstate) << endl;
    cout << "L2HITR: " << getL2CacheHitRatio(before_sstate, after_sstate) << endl;
    cout << "L3MPI: " << getIPC(before_sstate, after_sstate) << endl;
    cout << "L2MPI: " << getIPC(before_sstate, after_sstate) << endl;
    cout << "READ: " << getIPC(before_sstate, after_sstate) << endl;
    cout << "IO: " << getIPC(before_sstate, after_sstate) << endl;
    cout << "Instructoins: " << getInstructionsRetired(before_sstate, after_sstate) << endl;
    cout << "Cycles: " << getCycles(before_sstate, after_sstate) << endl;


    // cout << "Exec: " << getExecUsage(before_sstate, after_sstate) << endl;
    // cout << "IPC: " << getIPC(before_sstate, after_sstate) << endl;
    // cout << "Avg Freq: " << getAverageFrequency(before_sstate, after_sstate) << endl;
    // cout << "Active Avg Freq: " << getActiveAverageFrequency(before_sstate, after_sstate) << endl;
    // cout << "Cycles Lost L3 miss" << getCyclesLostDueL3CacheMisses(before_sstate, after_sstate) << endl;
    // cout << "Cycles Lost L2 miss" << getCyclesLostDueL2CacheMisses(before_sstate, after_sstate) << endl;
    // cout << "Relative Freq: " << getRelativeFrequency(before_sstate, after_sstate) << endl;
    // cout << "Active Relative Freq: " << getActiveRelativeFrequency(before_sstate, after_sstate) << endl;
    // cout << "L2 hit ratio: " << getL2CacheHitRatio(before_sstate, after_sstate) << endl;
    // cout << "L3 hit ratio: " << getL3CacheHitRatio(before_sstate, after_sstate) << endl;
    // cout << "L3 miss: " << getL3CacheMisses(before_sstate, after_sstate) << endl;
    // cout << "L2 miss: " << getL2CacheMisses(before_sstate, after_sstate) << endl;
    // cout << "L2 cache hits: " << getL2CacheHits(before_sstate, after_sstate) << endl;
    // cout << "L3 cache hits no snoop: " << getL3CacheHitsNoSnoop(before_sstate, after_sstate) << endl;
    // cout << "L3 cache hits snoop: " << getL3CacheHitsSnoop(before_sstate, after_sstate) << endl;
    // cout << "L3 cache hits: " << getL3CacheHits(before_sstate, after_sstate) << endl;
    // cout << "" << getInvariantTSC(before_sstate, after_sstate) << endl;
    // cout << "" << getRefCycles(before_sstate, after_sstate) << endl;

    m->cleanup();


  // PCM *m = PCM::getInstance();
  // m->program (PCM::DEFAULT_EVENTS, NULL);
  // SystemCounterState before_sstate = getSystemCounterState();
  // // Begin of custom code
  // cout<<"Starting simulation" << endl;
  //
  // simulate();
  //
  // cout<<"Simulation finished!!" << endl;
  // // End of custom code
  // SystemCounterState after_sstate = getSystemCounterState();
  //
  // cout << "Instructions per clock: " << getIPC(before_sstate,after_sstate) << endl;
  // cout << "L3 Cache Misses: "<< getL3CacheMisses(before_sstate,after_sstate) << endl;
  // cout << "L2 Cache Misses: "<< getL2CacheMisses(before_sstate,after_sstate) << endl;
  // cout << "L3 cache hit ratio: " << getL3CacheHitRatio(before_sstate,after_sstate) << endl;
  // cout << "L2 cache hit ratio: "<< getL2CacheHitRatio(before_sstate, after_sstate) << endl;
  // cout << "Bytes read:" << getBytesReadFromMC(before_sstate,after_sstate) << endl;

  // m->cleanup();

}
