
import numpy as np
import matplotlib.pyplot as plt


plt.rcParams['font.sans-serif']=['SimHei'] #显示中文标签 
plt.rcParams['axes.unicode_minus']=False


energy_50 = [1.51793, 1.51876, 1.51946, 1.52078, 1.52192, 1.52312]
energy_60 = [1.78291, 1.784, 1.78518, 1.78618, 1.78699, 1.78816]
energy_70 = [2.02964, 2.03144, 2.033, 2.03393, 2.03538, 2.03614]

wait_50 = [3948, 3988, 3888, 3740, 3712, 3636]
wait_60 = [4848, 4548, 4604, 4516, 4396, 4316]
wait_70 = [5524, 5332, 5336, 5140, 5080, 4920]


fig, axs = plt.subplots(1,3)

labels = ['1', '10', '20', '30', '40', '50']

x = np.arange(0, 6, 1)
axs[0].plot(x, energy_50, '-x')

axs[0].set_ylabel('能耗/10^4kw·h')


axs0 = axs[0].twinx()
axs0.plot(x, wait_50, '-o', color='green')

line1, = axs[1].plot(x, energy_60, '-x', label='能耗')
axs1 = axs[1].twinx()
line2, = axs1.plot(x, wait_60, '-o', color='green', label='总唤醒时间')
# axs[1].legend(loc='upper left')
# axs1.legend(loc='upper right')

axs[2].plot(x, energy_70, '-x')
axs2 = axs[2].twinx()
axs2.plot(x, wait_70, '-o', color='green')
axs2.set_ylabel('总唤醒时长/s')


for i in range(3):
    axs[i].grid(True)
    axs[i].set_xticks(x) # 
    axs[i].set_xticklabels(labels)
    axs[i].yaxis.set_tick_params(rotation=-90)
    axs[i].set_xlabel('空闲时长参数/s\n 作业负载 {}'.format(i+1))

axs0.yaxis.set_tick_params(rotation=90)
axs1.yaxis.set_tick_params(rotation=90)
axs2.yaxis.set_tick_params(rotation=90)

plt.legend(handles=[line1, line2], fontsize='8')

fig.tight_layout()
plt.savefig("energy_wait.png",dpi=300, bbox_inches = 'tight')
