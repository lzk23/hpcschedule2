//////////////////////////////////////////////////////////////////////////////  
// 
//  Header file of 3 Level Fat Tree abstraction (with resource tracking)
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
#ifndef _FT3_H_
#define _FT3_H_

// XGFT(3; n,n,m; 1,n,n)
// But we do not keep full connectivity here!
// we use this for allocation so we only track 
// one hierarchical tree - for now

#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <sstream>
#include "energy.h"

using namespace std;

// LaaS manages Job objects
class Job {
 public:
  int id;
  int numHosts;
  int numLinks; // to be filled after the allocator
  int arrival;
  int length;
  int start;
  int actNumHosts;
  int numL1UpLinks;
  int numL2UpLinks;
  bool haswakeup = false;

  std::vector<int> L1Ids;
  std::vector<int> L2Ids;
  std::vector<int> L3Ids;
  
  Job(int i, int n) {
    id = i;
    numHosts = n;
  };
  Job(int i, int n, int _arrival, int _length, int _start){
    id = i;
    numHosts = n;
    arrival = _arrival;
    length = _length;
    start = _start;
  }
  //~Job(void);

  string get_string()
  {
    std::ostringstream oss;
    oss << "job id=" << id <<" N="<<numHosts<<" len="<<length<<" start="<<start;
    std::string str = oss.str();
    return str;
  }
};

class SubTree {
 public:
  int level;
  int idxInLevel;
  int idxInTree;
  int numDnPorts;
  int numFreeHosts; // total number of free hosts below this level
  int numIdleHosts; // numFreeHosts = numSleepHosts + numIdleHosts
  vector<int> dnPortJob; // 0 is not assigned. -1 is L2's partial alloc of child L1
  class SubTree *parent;
  // valid only to level = 1:
  double hostCost;
  double hostGroup;

  string name() {
    stringstream s;
    s << "L" << level << " idx: " << idxInLevel;
    return s.str();
  };
  
};

struct lessUsedSubTree {
  bool operator()(const SubTree* a, const SubTree* b) {
    int aFree = a->numFreeHosts;
    int bFree = b->numFreeHosts;
    return ((aFree < bFree) || 
            ((aFree == bFree) && a->idxInLevel < b->idxInLevel));
  }
};

#define setSubTreeByLessUsage set<SubTree*, lessUsedSubTree>
class ThreeLevelFatTree {
  setSubTreeByLessUsage L1ByLessUsage;
  setSubTreeByLessUsage L2ByLessUsage;

  // return 1 if fail to get enough free sub trees
  int getUnAllocatedSubTrees(SubTree *st, int num, vector<int> &idxes);
  int getUnAllocatedSubTrees(SubTree *st, int num, vector<int> &idleidxes, vector<int> &sleepidxes, Energy *energy);

 public:
  map<int, map<SubTree *, list<int> > > jobSubTreePorts;
  int l1Hosts;
  int l2Hosts;
  int l3Hosts;
  int M1, M2, M3, W2, W3;

  vector<SubTree> L1SubTreeByIdx;
  vector<SubTree> L2SubTreeByIdx;

  ThreeLevelFatTree(int m1, int m2, int m3, int w2, int w3); 

  inline int getNumHosts() {return(l3Hosts);};
  inline const setSubTreeByLessUsage &getSubTreeByLessUsageSet(int level) {
    if (level == 2) {
      return L2ByLessUsage;
    }
    return L1ByLessUsage;
  };

  SubTree *getSubTree(SubTree *st, int idx);
  
  void setJobNodeAllocation(Job* job, SubTree *st, int numSubTrees, Energy *energy);

  // return number of freed nodes
  int delJobNodeAllocation(int jobId);
  friend class SimAlgo;
  
  int hostid2subtreeidx(int hostid);
};

#endif // _FT3_H_
