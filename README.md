# cc2530
cc2530的作用
    一般单芯片（cc2530）只有空中协议，实现命令的下发与状态的上报,结束空中数
据发送与接收后，可进入睡眠模式。若含有双芯片，还需要一份串口协议，实现设备与
cc2530的通讯，若需要睡眠，添加两个唤醒引脚线，或是在数据接收完成后，重新配置
串口模式，使其为中断唤醒。

仓库简介：
    该仓库包含两个分支，分别是dimmer、zhongKong，每个分支都含有一个项目的工程，
这些项目都是基于cc2530协议栈进行开发。开发环境IAR9.10

dimmer是顺舟调光开关的对接      
zhongKong是中控智能锁的对接
