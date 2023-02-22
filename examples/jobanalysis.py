import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif']=['SimHei'] #显示中文标签 
plt.rcParams['axes.unicode_minus']=False
dt = 3600 # s
tnum = int((3600/dt)*24)

def readdata(file_name):
    arrival_list = [0]*tnum
    f = open(file_name, 'r')
    datas = f.readlines()
    # print(len(arrival_list))
    for line in datas:
        data = line.split(',')
        id = int(data[0])
        req = int(data[1])
        arrival = int(data[2])
        run_time = int(data[3])
        tslot = arrival//(dt)
        # print(str(arrival)+","+str(tslot))
        arrival_list[tslot] += 1
    f.close()
        
    return arrival_list

def plot_figure(s1, s2, s3):
    t = np.arange(0, tnum, 1)
    labels = []
    for i in range(24):
        labels.append(i)
    fig, ax = plt.subplots(1, 1)
    ax.plot(t, s1, '-ro', t, s2, '-gx', t, s3, '-b+')
    ax.set_xlim(0, tnum)
    ax.set_xticks(labels)
    ax.set_xlabel('时段/h')
    ax.set_ylabel('作业数量')
    ax.grid(True)
    ax.legend(labels=['作业负载 1', '作业负载 2', '作业负载 3'])

    # cxy, f = axs[1].cohere(s1, s2, 256, 1. / dt)
    # axs[1].set_ylabel('coherence')

    fig.tight_layout()
    # plt.show()
    plt.savefig("jobinfo.png", dpi=300, bbox_inches = 'tight')


file_names = ["baseline=50_runtime=[5,7200]_req=[3,60]", "baseline=60_runtime=[5,7200]_req=[3,60]", "baseline=70_runtime=[5,7200]_req=[3,60]"]
data_dic = {}

print("tnum:"+str(tnum))

for i, file in enumerate(file_names):
    data_dic[i] = readdata(file)

plot_figure(data_dic[0], data_dic[1], data_dic[2])
