
import numpy as np
import matplotlib.pyplot as plt


plt.rcParams['font.sans-serif']=['SimHei'] #显示中文标签 
plt.rcParams['axes.unicode_minus']=False


uli_no_l_50 = [64.47, 64.47, 64.47, 64.47, 64.47, 64.47]
uli_no_l_60 = [68.04, 68.04, 68.04, 68.04, 68.04, 68.04]
uli_no_l_70 = [71.63, 71.63, 71.63, 71.63, 71.63, 71.63]

uli_l_50 = [62.69, 62.69, 62.69, 62.69, 62.69, 62.69]
uli_l_60 = [66.26, 66.26, 66.26, 66.26, 66.26, 66.26]
uli_l_70 = [69.36, 69.36, 69.36, 69.36, 69.36, 69.36]


fig, axs = plt.subplots(1,3)

labels = ['1', '10', '20', '30', '40', '50']

x = np.arange(len(labels))  # the label locations
width = 0.35  # the width of the bars
axs[0].bar(x - width/2, uli_no_l_50, width, label='Laas', color='b', hatch="///")
axs[0].bar(x + width/2, uli_l_50, width, label='TA', color='g', hatch="xxx")
axs[0].set_ylim(0,75)
axs[0].set_ylabel('节点资源利用率/%')
axs[0].legend(loc='upper left')


axs[1].bar(x - width/2, uli_no_l_60, width, label='Laas', color='b', hatch="///")
axs[1].bar(x + width/2, uli_l_60, width, label='TA', color='g', hatch="xxx")
axs[1].set_ylim(0,75)

axs[2].bar(x - width/2, uli_no_l_70, width, label='Laas', color='b', hatch="///")
axs[2].bar(x + width/2, uli_l_70, width, label='TA', color='g', hatch="xxx")
axs[2].set_ylim(0,75)

for i in range(3):
    axs[i].set_xticks(x) # 
    axs[i].set_xticklabels(labels)
    axs[i].set_xlabel('空闲时长参数/s\n 作业负载 {}'.format(i+1))

# axs0.yaxis.set_tick_params(rotation=90)
# axs1.yaxis.set_tick_params(rotation=90)
# axs2.yaxis.set_tick_params(rotation=90)

# plt.legend(handles=[line1, line2], fontsize='8')

fig.tight_layout()
plt.savefig("Resource_uti.png",dpi=300, bbox_inches = 'tight')
