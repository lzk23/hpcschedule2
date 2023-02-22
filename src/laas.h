//////////////////////////////////////////////////////////////////////////////  
// 
//  Header file of Links as a Service - Allocation Service
//
//  Copyright (C) 2014 TO THE AUTHORS
//  Copyright (C) 2014 TO THE AUTHORS
//
// Due to the blind review this software is available for SigComm evaluation 
// only. Once accepted it will be published with chioce of GPLv2 and BSD
// licenses.  
// You are not allowed to copy or publish this software in any way.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef _LAAS_H_
#define _LAAS_H_

#include "ft3.h"
#include "isol.h"
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
#include <regex>



// namespace std {
//    template(VecInt) vector<int>;
//    template(PairInt) pair<int, int>;
//    template(VecPairInt) vector<pair<int,int> >;
//    template(SetInt) set<int>;
// };

// bool vector_contain(vector<int> &vecinput, int key){
//   if(find(allocate_hosts.begin(), allocate_hosts.end(), hostid) != allocate_hosts.end())
// }


class Reservation{
  public:
    Job* job;
    vector<int> hosts;
    vector<int> l1Up;
    vector<int> l2Up;
    int numLinks;
    string get_string(){
      std::ostringstream oss;    
      oss << "job id=" << (*job).id <<" hosts="<< vector_int_to_str(hosts, ",") <<" l1Up="<< vector_int_to_str(l1Up, ",") <<" l2Up="<< vector_int_to_str(l2Up, ",");
      std::string str = oss.str();
      return str;
    }
};


class JobReservations{
  public:
    priority_queue<int, vector<int>, greater<int> > sorted_times;
    priority_queue<int, vector<int>, greater<int> > sorted_times_descending;
    std::map<int,vector<Reservation*> > reservationsBytime;
    int get_num(){
      return reservationsBytime.size();
    }
    void insert(int t, Reservation* reservation);
    std::vector<Reservation*> pop(int &t);
    int minT(){
      if(sorted_times.size()==0)
        return -1;
      return sorted_times.top();
    };
    int maxT(){
      if(sorted_times.size()==0)
        return -1;
      return sorted_times.top();
    };
};

class RunningJobs
{
  public:
    // std::vector<int> times;
    std::map<int,vector<Job*> > jobsByTime;
    // 大顶堆, 大到小排序
    // priority_queue<int> queue;
    priority_queue<int, vector<int>, greater<int> > sorted_times;
    int get_num()
    {
      return jobsByTime.size();
    }
    void insert(int t, Job* job);
    std::vector<Job*> pop(int &t);
    int minT()
    {
      if(sorted_times.size()==0)
        return -1;
      return sorted_times.top();
    };
};

class LaaS {
 private:
  ofstream log;
  IsolAlgo *alg;
  ThreeLevelFatTree *ft;
  std::map<int, Job> jobByID;
  bool verbose;
  bool isBadState;
  std::ostringstream lastErrorMsg;

 public:
  LaaS();
  LaaS(std::vector<int> M, std::vector<int> W, std::string logFileName, Energy* energy);

  // if something really bad happen to the module - it will return false
  bool good() {return(!isBadState);};

  // Any function returning with non zero also provides the error message.
  // the return code is defined for each function separately
  string getLastErrorMsg();

  // set verbosity ON/OFF
  int setParam(bool v);

  // reload the log file
  int replayLogFile();

  // NOTE: in the below a switch port is defined as a pair< switch-idx, port-num >
  //       these are logical switch numbers and port numbers... 
  //       Switch index at L1 and L2 is from left to right assuming bipartites are close
  //       Switch index at L3 assumes L3 bipartites are close 

  // get all currently unused switch ports
  int getUnAllocated(std::vector<int> &hosts,           // OUT Hosts
                     std::vector<std::pair<int, int> > &l1UpPorts,  // OUT unallocated L1 switch up ports
                     std::vector<std::pair<int, int> > &l2UpPorts); // OUT unallocated L2 switch up ports

  // get all active tenant ids
  int getTenants(std::set<int> &tenantIds);

  // get the resources allocated to the tenant. return 1 if tenant does not exist
  int getTenantAlloc(int tenantId,
                     std::vector<int> &hosts,           // OUT allocated host indexies
                     std::vector<std::pair<int, int> > &l1UpPorts,  // OUT allocated L1 switch up ports
                     std::vector<std::pair<int, int> > &l2UpPorts); // OUT allocated L2 switch up ports

  // try to allocate the tenant
  int allocTenant(Job* job); // IN number of hosts per each leaf group index
  
  // manually assign tenant resources
  int assignTenant(int tenantId,
                   std::vector<int> &hosts,                       // IN allocated host indexies
                   std::vector<std::pair<int, int> > &l1UpPorts,  // IN allocated L1 switch up ports
                   std::vector<std::pair<int, int> > &l2UpPorts); // IN allocated L2 switch up ports

  // remove a tenant
  int deallocTenant(Job* job, vector<int> &deallohosts);

  // set the group each leaf
  int setLeafGroup(int group, std::vector<int> leafIdxs);
  
  // set the cost of hosts on each leaf
  int setLeafCost(int group, std::vector<double> leafIdxs);

  void updateIdleNum(std::vector<int> hostid, Energy* energy);

  int getavailableHosts(){
    return alg->availableHosts;
  }
  
};

class Sim
{
  public:
    std::vector<int> M;
    std::vector<int> W;
    std::vector<Job*> pending;
    std::vector<Job*> jobs;
    RunningJobs running;
    JobReservations reserved;
    LaaS engine;
    Energy *energy;
    double totalenergy = 0;
    int add_job_waiting_time = 0;
    double run_energy_without_wakeup = 0; //not include wakeup
    int lastJobPlacement = -1;
    int firstJobWaiting = -1;
    int lastJobEnd = -1;
    double runTime = 0;
    bool verbose;

    int parseJobCsv(string csvFileName);
    void getReservation();
    void run();
    void analyze();
    
    Sim(std::vector<int> &M, std::vector<int> &W, string jobsCsv, bool verbose);
};




#endif // _LAAS_H_
