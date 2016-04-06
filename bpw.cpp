#include <iostream>
#include "intel/cpucounters.h"

#define F 200
#define C 200

using namespace std;

int main() {
  PCM *m = PCM::getInstance();
  m->program (PCM::DEFAULT_EVENTS, NULL);
  SystemCounterState before_sstate = getSystemCounterState();
  cout << "get L2Miss"<<getL2Miss(before_sstate)<<endl;
  cout << "get L2Re"<<getL2Ref(before_sstate)<<endl;
  cout << "get L3Miss"<<getL3Miss(before_sstate)<<endl;
  cout << "get L3UnsharedHit"<<getL3UnsharedHit(before_sstate)<<endl;
  cout << "get L2HitM"<<getL2HitM(before_sstate)<<endl;
  cout << "get L2Hit"<<getL2Hit(before_sstate)<<endl;
  m->cleanup();

}
