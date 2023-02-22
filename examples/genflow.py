import numpy as np
import sys 
import os
# sys.path.append("..") 
# from config import config

days = 1
hours = 24
baseline = 100
rate = [0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2, 0.5, 1, 1.5, 2, 2, 1.5, 1, 1, 1.5, 1.5, 1.5, 1.5, 1, 1, 2, 2, 1.5]
span_time_ratio = 0.2

run_time_max = 7200
run_time_min = 5
request_node_max = 60
request_node_min = 3

arrival_max = days*hours*3600


def generation():
    save_file_name = 'baseline={}_runtime=[{},{}]_req=[{},{}]'.format(baseline, run_time_min, run_time_max, request_node_min, request_node_max)
    
    f = open(save_file_name, mode='w')

    id = 1
    current_time = 0

    for d in range(days):
        for h in range(hours):
            basetime = (3600 / (baseline * rate[h]))
            arrival_time_min = int(basetime * (1 - span_time_ratio))
            arrival_time_max = int(basetime * (1 + span_time_ratio))
            start_time = 3600 * h

            while current_time - start_time <= 3600:
                arrival_time = current_time + np.random.randint(arrival_time_min, arrival_time_max+1)
                
                if arrival_time >= arrival_max:
                    print(str(arrival_time)+","+str(arrival_max))
                    break

                current_time = arrival_time
                run_time = np.random.randint(run_time_min, run_time_max+1)
                request_node = np.random.randint(request_node_min, request_node_max+1)

                f.write('{}, {}, {}, {}\n'.format(id,request_node, arrival_time,run_time))
                id += 1
            
    f.close()
    print("succeed!")

if __name__ == '__main__':
    generation()