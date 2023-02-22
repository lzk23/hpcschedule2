#include "energy.h"
#include <algorithm>    // std::find
#include <limits.h>
//using namespace std;

Energy::Energy(std::vector<int> &_M){
  int M1 = _M[0];
  int M2 = _M[1];
  int M3 = _M[2];
  // sta2power = {{run, 100}, {idle, 51}, {sleep, 8}};
  sta2power[run] = 100;
  sta2power[idle] = 51;
  sta2power[sleepstate] = 8;
  sleep2run = 4;
  extern int g_maxidletime;
  maxidletime = g_maxidletime;
  std::vector<int> idle_time(M1);
  StateATime sat = {idle, 0};
  HostsStateATime.resize(M3 * M2 * M1, sat);
}

// std::vector<int>
// Energy::getNeedSleepHosts(){
  
// };

//分配的节点和移除作业的节点可能有相同的，先移除再分配嘛
void
Energy::updateStateATime(std::vector<int> &allocatehosts, std::vector<int> &deallocatehost, std::vector<int> &sleephostids){
  
  for(int i = 0; i < HostsStateATime.size(); i++){
    if((find(allocatehosts.begin(), allocatehosts.end(), i) == allocatehosts.end() &&
       find(deallocatehost.begin(), deallocatehost.end(), i) == deallocatehost.end()) ||
       find(lockhosts.begin(), lockhosts.end(), i) != lockhosts.end()){
            HostsStateATime[i].dtime += 1;
            //calcualte energy
            if(HostsStateATime[i].sta == run){
              run_e_with_wakeup += sta2power[run];
            }
            if(HostsStateATime[i].sta == idle){
              idle_e += sta2power[idle];
            }
            if(HostsStateATime[i].sta == sleepstate){
              sleep_e += sta2power[sleepstate];
            }
            if(HostsStateATime[i].sta == idle){
              if(HostsStateATime[i].dtime >= maxidletime){
               HostsStateATime[i].sta = sleepstate;
               HostsStateATime[i].dtime = 0;
               sleephostids.push_back(i);
              }
            }
       }
  }
}

void
Energy::updatedeallocate(std::vector<int> &deallohosts){

  for(auto i : deallohosts){
    if(HostsStateATime[i].sta != run &&
    find(lockhosts.begin(), lockhosts.end(), i) == lockhosts.end()){
        throw "error";
    }
    HostsStateATime[i].sta = idle;
    HostsStateATime[i].dtime = 0;

    std::vector<int>::iterator it;
    for(it = lockhosts.begin(); it != lockhosts.end(); it++){
      if(*it == i){
        lockhosts.erase(it);
        break;
      }
    }
  }
}

void
Energy::updateallocate(std::vector<int> &allohosts, int endlockidx){
  for(int i=0; i < endlockidx; i++){
    int hostid = allohosts[i];
    if(HostsStateATime[hostid].sta == run){
        throw "error";
    }
    HostsStateATime[hostid].sta = run;
    HostsStateATime[hostid].dtime = 0;
  }
  for(int i=endlockidx; i< allohosts.size(); i++){
    int hostid = allohosts[i];
    if(HostsStateATime[hostid].sta == run){
      throw "error";
    }
    if(find(lockhosts.begin(), lockhosts.end(), hostid) != lockhosts.end()){
      throw "error";
    }
    lockhosts.push_back(hostid);
  }
}


void
Energy::setState(int hostid, state sta, int time){
  HostsStateATime[hostid].sta = sta;
  HostsStateATime[hostid].dtime = time;
}

state
Energy::getState(int hostid){
  if(hostid>HostsStateATime.size()){
    // std::cout << "-E- the host id " << hostid << "is error" << std::endl;
    // exit(1);
    throw "error";
  }
  return HostsStateATime[hostid].sta;
}

void
Energy::set2Run(std::vector<int> needRun){
  std::vector<int>::const_iterator it;
  for(it=needRun.begin(); it != needRun.end(); it++){
    if(HostsStateATime[*it].sta == run){
      throw "error";
    }
    setState(*it, run, 0);
  }
}

int
Energy::getSleep2runtime(){
  return sleep2run;
}

bool
Energy:: inlock(int hostid){
  if(find(lockhosts.begin(), lockhosts.end(), hostid) == lockhosts.end()){
    return false;
  }
  return true;
}

int 
Energy::getonenodewakeupenergy(){
  return sleep2run*sta2power[run];
}

int
Energy::getrunpower(){
  return sta2power[run];
}

int
Energy::getaddtionalenergy(){
  return (sta2power[run]-sta2power[idle])*sleep2run;
}

