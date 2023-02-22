#include <iostream>
#include <vector>
#include <unistd.h>
#include <laas.h>
#include <stdio.h>
#include <string.h>
#include <cassert>

using namespace std;

Sim::Sim(std::vector<int> &_M, std::vector<int> &_W, string jobsCsv, bool v)
{
  M = _M;
  W = _W;
  parseJobCsv(jobsCsv);
  running = RunningJobs();
  verbose = v;
  energy = new Energy(_M);
  engine = LaaS(M, W, "logfile", energy);
  if(!engine.good())
    throw "error";
  engine.setParam(verbose);
  lastJobPlacement = -1;
  firstJobWaiting = -1;
  runTime = 0;
}

int
Sim::parseJobCsv(string csvFileName)
{
  ifstream inFile(csvFileName);
  string lineStr;
  while(getline(inFile, lineStr)){
    stringstream ss(lineStr);
    string str;
    vector<string> lineArray;
    while(getline(ss, str, ',')){
      lineArray.push_back(str);
    }
    int id = atof(lineArray[0].c_str());
    int N = atof(lineArray[1].c_str());
    int arrival = atof(lineArray[2].c_str());
    int length = atof(lineArray[3].c_str());
    Job* job = new Job(id, N, arrival, length, -1);
    pending.push_back(job);
    jobs.push_back(job);
  }
  return 0;
};


void
Sim::run()
{
  clock_t start, end;
  start = clock();
  int t = 0;
  
  // calculate total run time, because we append the wakeup time in the run time
  for(auto j: jobs){
    run_energy_without_wakeup += j->numHosts*(j->length)*energy->getrunpower();
  }

  while (pending.size()||running.get_num()){
    if(verbose)
        std::cout<<"-I- At t="<< t<<endl;
      
    // remove the jobs that have finished
    int endT = running.minT();
    vector<int> deallohosts;
    while (endT != -1 & endT <= t){
        std::vector<Job*> endingJobs = running.pop(endT);
        for(int j=0; j < endingJobs.size(); j++){
            if(!engine.deallocTenant(endingJobs[j], deallohosts))
                throw "error";
            else{
                if(verbose)
                    cout << "-V- Remove " << endingJobs[j]->get_string() << endl;
            }   
        }
        endT = running.minT();
    }
    energy->updatedeallocate(deallohosts);
    bool placeFailed = false;
    vector<int> allocatehosts;
    
    if(pending.size()){
        Job* job = pending[0];
        while(!placeFailed&job->arrival<=t){
            placeFailed = engine.allocTenant(job);
            if(placeFailed){
                if(running.get_num()<=0)
                    //throw "No job running but fail to place job %s" + job.get_string();
                    throw "error";
                else{
                    if(firstJobWaiting < 0 & t > 0)
                        firstJobWaiting = t;
                    if(verbose)
                        cout << "-V- Fail " << job->get_string() << endl;
                }
                
                int wait_num = 0;
                std::vector<Job*>::iterator iter;
                for(iter=pending.begin(); iter < pending.end(); iter++){
                if((*iter)->arrival <= t){
                    wait_num++;
                }
                else{
                    break;}
                }

                cout << "-Place Fail job:" << job->id << ", req:" << job->numHosts << ", ava:" << engine.getavailableHosts() << ", wait:" << wait_num << ", pending:"<< pending.size() << endl;

            }
            else{            
                vector<pair<int, int> > l1Up;
                vector<pair<int, int> > l2Up;
                vector<int> allocatehostscurrent;

                engine.getTenantAlloc(job->id, allocatehostscurrent, l1Up, l2Up);
                job->actNumHosts = allocatehostscurrent.size();
                allocatehosts.insert(allocatehosts.end(), allocatehostscurrent.begin(), allocatehostscurrent.end());

                energy->updateallocate(allocatehostscurrent, job->numHosts);

                // if(job->actNumHosts - job->numHosts >= M[0]){
                //     cout << "-E- --------job id:" << job->id << ", " << (job->actNumHosts-job->numHosts) << endl;
                // }
                // if(job->actNumHosts > job->numHosts){
                //     cout << "job id:" << job->id << ", " << (job->actNumHosts-job->numHosts) << endl;
                // }

                job->numL1UpLinks = l1Up.size();
                job->numL2UpLinks = l2Up.size()*W[1];

                if(verbose)
                    cout << "-V- Place " << job->get_string() << " L1=" << job->numL1UpLinks << " L2=" << job->numL2UpLinks << endl;

                running.insert(t + job->length +1 , job);
                job->start = t;
                lastJobPlacement = t;
                pending.erase(pending.begin()); 
                if(pending.size()>0){
                    job = pending[0];
                }
                else
                    placeFailed = true;
            }
        }
    }

    std::vector<int> sleephostids;
    energy->updateStateATime(allocatehosts, deallohosts, sleephostids);
    engine.updateIdleNum(sleephostids, energy);
    

    // if(placeFailed)
    //     t = running.minT();
    // else
    //     t = pending[0]->arrival;
    lastJobEnd = t;
    t = t + 1;
  }
  
  double run_energy_with_wakeup = 0;
  double run_energy_thatis_wakeup = 0;
  for(auto j: jobs){ 
    run_energy_with_wakeup += j->numHosts*(j->length)*energy->getrunpower();
    if(j->haswakeup){      
        run_energy_thatis_wakeup += j->numHosts*energy->sleep2run*energy->getrunpower();
        add_job_waiting_time += energy->sleep2run;
    }
  }

  cout << "run_energy_without_wakeup:" << run_energy_without_wakeup << " run_energy_with_wakeup:" << run_energy_with_wakeup << " run_energy_thatis_wakeup:" << run_energy_thatis_wakeup <<
  " sum first and third terms:" << (run_energy_without_wakeup + run_energy_thatis_wakeup) << endl;

  end = clock();
  runTime = (end - start)/CLOCKS_PER_SEC;
}


void
Sim:: analyze(){
    // if(firstJobWaiting < 0){
    //     cout << "-E- Can't provide analysis when no job waited, firstJobWaiting=" << firstJobWaiting << endl;
    //     return;
    // }
    extern int g_maxidletime;
    extern bool g_uselesstree;
    extern string g_csvFileName;
    printf("m:%s, w:%s, t:%d, l:%d\n", vector_int_to_str(M,",").c_str(), vector_int_to_str(W,",").c_str(), g_maxidletime, g_uselesstree);
    printf("data:%s\n", g_csvFileName.c_str());


    std::cout << "-I- first waiting job at:" << firstJobWaiting << " lastJobPlacementTime:" << lastJobPlacement << " lastJobEnd:" << lastJobEnd << endl;
    int numHosts = M[0] * M[1] * M[2];
    int numL1UpLinks = M[2] * M[1] * W[1];
    int numL2UpLinks = M[2] * W[1] * W[2];

    int totalTime = 1.0*(lastJobEnd);
    double potentialHostTime = totalTime * numHosts;
    double actualHostTime = 0.0;
    double potentialL1UpLinksTime = totalTime * numL1UpLinks;
    int actualL1UpLinksUsed = 0.0;
    double potentialL2UpLinksTime = totalTime * numL2UpLinks;
    double actualL2UpLinksUsed = 0.0;
    double potentialTotalLinksTime = potentialHostTime + potentialL1UpLinksTime + potentialL2UpLinksTime;
    double actualTotalLinksUsed = 0.0;

    std::cout << "-I- Total potential hosts * time =" << potentialHostTime << endl;
    int nJobs = 0;
    int skipLast = 0;
    int skipFirst = 0;
    double totalruntime = 0;
    
    for(auto j : jobs){
        int runtime = j->length;
        // if(j->start + j->length < firstJobWaiting){
        //     skipFirst = skipFirst + 1;
        //     continue;
        // }
            

        // if(j->start >= lastJobPlacement){
        //     skipLast = skipLast + 1;
        //     continue;
        // }
        
        //for those starting before the first wait - take only the 
        //time after the start
        // if(j->start < firstJobWaiting)
        //     runtime = j->start + runtime - firstJobWaiting;
            
        //if crossing the stop point only take the runtime before
        if(j->start + runtime > lastJobEnd)
            //runtime = lastJobPlacement - j->start;
            throw "error";

        nJobs = nJobs + 1;

        actualHostTime += 1.0 * runtime * j->numHosts;
        actualL1UpLinksUsed += runtime * j->numL1UpLinks;
        actualL2UpLinksUsed += runtime * j->numL2UpLinks;
        actualTotalLinksUsed += runtime * (j->actNumHosts + j->numL1UpLinks +  j->numL2UpLinks);

        // if(j->actNumHosts != j->numHosts){
        //     throw "error";
        // }
    }

    // end of for

    printf("-I- Total considered jobs: %d skip first: %d last: %d\n", nJobs, skipFirst, skipLast);
    printf("-I- Total actual hosts * time = %g\n", actualHostTime);
    double hl =  (100.0*actualHostTime)/potentialHostTime;
    printf("-I- Host Utilization = %3.2f %%\n", hl);
    double l1u = (100.0*actualL1UpLinksUsed)/potentialL1UpLinksTime;
    printf("-I- L1 Up Links Utilization  = %3.2f %%\n", l1u);
    double l2u = (100.0*actualL2UpLinksUsed)/potentialL2UpLinksTime;
    printf("-I- L2 Up Links Utilization  = %3.2f %%\n", l2u);
    double ltu = (100.0*actualTotalLinksUsed)/potentialTotalLinksTime;
    printf("-I- Total Links Utilization  = %3.2f %%\n", ltu);
    printf("-I- Run Time = %3.3g sec\n", runTime);

    double run_e_without_wakeup = energy->run_e_with_wakeup - energy->wakeup_e - energy->idle_seemas_run_e;
    double idleenergy = energy->idle_e + energy->addtional_idle_e;
    
    cout << "run energy thatis wakeup calculated by steps:" << (energy->wakeup_e + energy->idle_seemas_run_e) << endl;
    cout << "run energy with wakeup calcualted by steps:" << energy->run_e_with_wakeup << endl;
    cout << "Energy of each part, run without wakeup:" << run_e_without_wakeup << ", idle:" << idleenergy << ", sleep:" << energy->sleep_e << ", wakeup:" << energy->wakeup_e << endl;
    totalenergy = run_e_without_wakeup + idleenergy + energy->sleep_e;
    printf("-I- Total consume energy = %g add_job_waiting_time=%d\n", totalenergy, add_job_waiting_time);
}

bool g_uselesstree = false;
int g_maxidletime = 10;
string g_csvFileName;

int main(int argc,char * argv[])
{
    bool verbose = false;
    
    std::vector<int> M;
    std::vector<int> W;
    int o;
    const char *optstring = "m:w:vc:lt:"; // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
    const char *split = ",";
    while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 'm':
                printf("opt is m, oprarg is: %s\n", optarg);
                M = str_to_int(optarg);       
                break;
            case 'w':
                W = str_to_int(optarg);  
                printf("opt is w, oprarg is: %s\n", optarg);
                break;
            case 'v':
                verbose = true;
                printf("opt is v, oprarg is: %d\n", verbose);
                break;
            case 'c':
                g_csvFileName = optarg;
                printf("opt is c, oprarg is: %s\n", optarg);
                break;
            case 'l':
                g_uselesstree = true;
                printf("opt is l, oprarg is: %d\n", g_uselesstree);
                break;
            case 't':
                g_maxidletime = atof(optarg);
                printf("opt is t, oprarg is: %d\n", g_maxidletime);
                break;
            case '?':
                printf("error optopt: %c\n", optopt);
                break;
        }
    };

    Sim sim = Sim(M, W, g_csvFileName, verbose);
    sim.run();
    sim.analyze();

    return 0;
};
