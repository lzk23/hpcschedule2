#ifndef _ENERGY_H_
#define _ENERGY_H_

#include "portmask.h"
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <queue>
#include <time.h>
#include <iostream>
#include <iterator>

enum state {run, idle, sleepstate};

struct StateATime{
    state sta;
    int dtime;
};

class Energy{
    private:
     
     std::vector<int> lockhosts;
          
    public:
    //IsolAlgo *alg;
     std:: map<state, int> sta2power;
     int sleep2run;
     int maxidletime;
     std::vector<StateATime> HostsStateATime;
     double run_e_with_wakeup = 0;
     double idle_e = 0;
     double sleep_e = 0;
     double wakeup_e = 0;
     double idle_seemas_run_e = 0; //because we append the wakeup time in the run time, however, for the idle nodes, there are no wakeup time
     double addtional_idle_e = 0;
     Energy();
     Energy(std::vector<int> &M);
    //std::vector<int> getNeedSleepHosts();
    //void set2Sleep(std::vector<int> needSleep);
    //void set2Idle(std::vector<int> needIdle);
     void set2Run(std::vector<int> needRun);
     void updateStateATime(std::vector<int> &allocatehosts, std::vector<int> &deallocatehost, std::vector<int> &sleephostids);
     void updatedeallocate(std::vector<int> &deallohosts);
     void updateallocate(std::vector<int> &allohosts, int endlockidx);
     state getState(int hostid);
     void setState(int hostid, state sta, int t);
     int getSleep2runtime();
     bool inlock(int hostid);
     int getonenodewakeupenergy();
     int getrunpower();
     int getaddtionalenergy();
     //void revise
};
#endif