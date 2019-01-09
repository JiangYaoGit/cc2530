2017/02/10 11:12:29 
# ZigBee/SmartRoom Core Release Notes #
Wulian, Inc.

ZigBee/SmartRoom Core Release Notes

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------

# Version 1.1.2b #
September 14, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##
 - [1] 增加单火线设备的加网时禁止开关操作api；
 
 - [2] 增加发射功率可配合并且保存命令；
 
 - [3] 修改/C/UUeXXXX命令查询双向信号值指示灯不闪烁
 

  
# Version 1.1.2a #
September 14, 2016
## Changes:##
 - [2 增加退网成功后回调函数。

# Version 1.1.2 #
September 14, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##

 - [1] NO
 - 
## Bug Fixes: ##

 - [1] 修复零火绑定开关由于删除EndDevice设备关联表造成设备的绑定控制失败问题；

 - [2] 修复设备电池电压检测参数错误造成检测上报参数错误问题；

## Known Issues: ##
 - [1] 设备入网后重新上电偶尔会出现丢失RS信息的问题。；

 - [2] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

----------
# Version 1.1.1 #
August 05, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##

 - [1] NO
 - 
## Bug Fixes: ##

 - [1] 修复零火绑定开关和单火线开关绑定控制时，由于单火线开关切换父设备造成的不能控制的问题；

 - [2] 修复固定周期心跳的aps ack和直接到网关的心跳问题；
 - 
 - [3] 解决R0042E网关模块固件使用串口1时P1.5引脚不能够正常驱动的问题

## Known Issues: ##

 - [1] 设备入网后重新上电偶尔会出现丢失RS信息的问题。；
 - 
 - [2] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。
 - 
----------
# Version 1.1.0 #
August 05, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##

 - [1] 增加默认是否选择禁止产测的功能；

 - [2] 修改EndDevice设备的父设备是网关的情况下不在发送额外的心跳。

 - [3] 将首个带有aps ack的心跳延迟10min开始。

 - [4] 增加设备的基础库版本号以及应用版本号查询，使用命令/C/UUmXXXX
## Bug Fixes: ##

 - [1] 解决上电自动入网和不自动入网的共存问题。

 - [2] 解决设备使用命令退网255s在UserAppAutoStartNwk()返回值为true时设备会自动加网的问题。

 - [3] 解决路由设备在网关不通时能够自行判断是否掉线

 - [4] 解决某种情况下会出项应用层数据丢失，网络假死的现象。

 - [5] 解决设备产测完成灯闪未结束之前直接掉电造成设备重新上电后不自动搜网的问题。

 - [6] 修复额外到网关心跳和指定心跳APS ack一段时间之后周期不对应的问题

 - [7] 解决具有绑定功能的设备，在被绑定设备退网重新加网后，重新建立被绑定设备和绑定设备之间的绑定关系后，绑定不可控的问题。

## Known Issues: ##
 - [1] 设备入网后重新上电偶尔会出现丢失RS信息的问题。；

 - [2] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。
 
----------

# Version 1.0.13 #
July 14, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##

 - [1] 增加通过XXXXUserApp.c文件中增加UserAppPowerOnJoinNwkDelayCallBack()来实现设备上电入网启动延时；

 - [2] 增加心跳回调机制，为一些特定需求下在心跳回调中处理任务；

 - [3] 网关增加华三定制新模块的特性


## Bug Fixes: ##

 - [1] 解决由于版本库升级造成的UserAppStartNwkJoiningCallBack()函数回调失效的问题。

## Known Issues: ##
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗；

 - [2] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [3] 设备的网络层会在某种极限条件下假死，无线数据不能够收发但是本地控制可以继续进行。

 - [4] 具有绑定功能的设备，在被绑定设备退网重新加网后，重新建立被绑定设备和绑定设备之间的绑定关系后，绑定不可控


----------

# Version 1.0.12 #
July 13, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##

 - [1] 增加RS刷新命令接口void WZCL_SendRsMessage(void)；

 - [2] 取消发送绑定消息指定aps ack，因此所有具有绑定功能的开关，包括零火绑定，和场景开关都需要更新；

 - [3] 为老版本的产品升级配置不带产测功能的基础库,解决库存老产品升级问题。

 - [4] 将心跳配置成可配置的，默认值为1min

 - [5] 生成15dBm、18dBm、20dBm的带PA设备的基础库各一套。

 - [6] 解决由于版本库升级造成的UserAppStartNwkJoiningCallBack()函数回调失效的问题。


## Bug Fixes: ##

 - [1] 解决零火线绑定开关出现的工作一段时间之后会解绑的问题；

 - [2] 解决电池设备由于压降器件造成的电池电压获取不准确的问题。

## Known Issues: ##
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗；

 - [2] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [3] 设备的网络层会在某种极限条件下假死，无线数据不能够收发但是本地控制可以继续进行。

 - [4] 具有绑定功能的设备，在被绑定设备退网重新加网后，重新建立被绑定设备和绑定设备之间的绑定关系后，绑定不可控

 - [5]

----------



# Version 1.0.11 #
June 07, 2016

## Notices: ##

 - [1] 提交更新了新的版本的基础库

## Changes:##

 - [1] 验证了电池设备的新的电压检测算法，为一些由于防反接器件造成的芯片读取的电压较测量电压低增加了压降修正偏移量如下面的修正压降0.2V `devInfo->lowBatteryAlarm.VoltageOffset = 20；`并且将读取到的电池设备的电压写入到Flash；

 - [2] 增加Router设备能够通过定期交互知道Coordinator设备的网络状态，并且通过回调函数void UserAppCoordinatorOfflineCallback(bool offline)通知上层Coordinator是否在网络中；

 - [3] 增加绑定控制的aps ack 确保绑定控制的可靠性。


## Bug Fixes: ##

 - [1] 解决零火线绑定开关出现的工作一段时间之后会解绑的问题；

 - [2] 解决电池设备由于压降器件造成的电池电压获取不准确的问题。

## Known Issues: ##
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗；

 - [2] 使用/C/UUeXXXX获取的设备的双向信号值时，在上行信号值到负九十几时会出现信号值异常问题；

 - [3] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

----------
# Version 1.0.10 #
June 07, 2016



## Notices: ##

 - [1] 提交更新了新的版本的基础库



## Changes:##

 - [1] 统一老的按键加网方式和新的上电自动加网方式；

 - [2] 老的加网退网方式使用的lib为Projects\zstack\Libraries\TI2530DB\bin路径下不带有-AutoStart的库；

 - [3] 增加上电自动入网的lib，在Projects\zstack\Libraries\TI2530DB\bin路径下带-AutoStart；

 - [4] 使用老的上电加网方式需要定义宏HOLD_AUTO_START，使用新的加网方式需要取消宏HOLD_AUTO_START的定义；

 - [5] 根据华三的项目需要增减产品表示码的存储协议/C/UUcXXXXBBBB 写入标识码  /C/UUcXXXX读取标识码。




## Bug Fixes: ##

 - [1] 解决上电自动加网方式的BUG问题。



## Known Issues: ##

 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume
 成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗

 - [2] 路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [3] 路由频繁控制时间久了之后会出现路由器应用层能够正常控制，无线数据不能够发出的问题。
 -
 - [4] 零火线绑定开关使用过程中会出现自动解绑现象。

Version 1.0.9
May 27, 2016

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
  1、将可用的按键个数有8个增加到16个。

  2、电池电压检测方式变更。还未经过验证，因此本版不作为正式产品开发版本，仅提供给
  陈刚作为遥控器的开发测试程序，其他产品需要使用需要等待更新
Bug Fixes:

Known Issues:
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume
 成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗

 - [2]路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [3]路由频繁控制时间久了之后会出现路由器应用层能够正常控制，无线数据不能够发出的问题。

Version 1.0.8
March 01, 2016

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
  1、修改加网方式为上电自动入网。长按时间可配置退网，默认长按6s退网，退网后不自动加网
  按一下SET键或者快按4次SET键入网。

  2、默认网络层信息重发次数由5次改为4次。

  3、路由PA设备的发射功率改为+15dBm

Bug Fixes:

Known Issues:
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume
 成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗

 - [2]路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [3]路由频繁控制时间久了之后会出现路由器应用层能够正常控制，无线数据不能够发出的问题。

Version 1.0.7
March 01, 2016

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
  1、修改按键长按退网时间可配置。

Bug Fixes:
 - [1] 采用新的EndDevice-pro.lib解决EndDevice 设备在加入到一个网络中，Coordinator
 设备在同一信道重新建网后，EndDevice会跟随Coordinator设备Rejion到新的PanId的问题。

Known Issues:
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume
 成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗

 - [2]路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [3]路由频繁控制时间久了之后会出现路由器应用层能够正常控制，无线数据不能够发出的问题。

Version 1.0.6
March 01, 2016

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
 - [1]增加了路由设备的APS ack命令，当路由设备收到指定网关的Aps Ack命令之后，会直
 接闪烁sys指示灯，直到收到下一次指定的APS ack。

 - [2]增加用于EndDevice的OS应答的测试命令/C/UUYXXXX1该命令同时适用于路由设备和网关设备。


 - [3]增加EndDevice设备aps ack的测试专用命令。

 - [4]将EndDevice设备的丢失网络时间用系统时间来计算，在发货版本中同样具有此功能。

 - [5]在协议栈中增加固件生成时间自动添加代码，固件中的固件生成时间与实际生成时间一致，
 同时不需要人为修改。

 - [6]增加看门狗功能的自定义设置功能，将看门狗函数接口开放出来了，开发者可自行启用或
 者关闭开门狗功能。

 - [7]增加路由设备从关联表中清除一个EndDevice设备之后上报一个ES数据,正常出货时也有。

Bug Fixes:
 - [1] 采用新的EndDevice-pro.lib解决EndDevice 设备在加入到一个网络中，Coordinator
 设备在同一信道重新建网后，EndDevice会跟随Coordinator设备Rejion到新的PanId的问题。

Known Issues:
 - [1] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume
 成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗

 - [2]通信正常，突然resume的原因,可能是没有收到MAC的应答导致设备的复位

 - [3]路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

 - [4]路由频繁控制时间久了之后会出现路由器应用层能够正常控制，无线数据不能够发出的问题。

Version 1.0.5
February 02, 2016

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
 - [1]增加设备退网是在网络层发送leave命令

 - [2]优化设别Join时扫描每个信道的时间

 - [3]优化设备由于MTO失败导致不能控制问题，增加EndDevice设备设置定期直接向Coordinator
 发送心跳的入口。

 - [4]增加EndDevice设备丢失网络的时间打印机制，测试专用，发货程序中没有此功能.

 - [5]修改nwk重发次数为5次，用来保证设别在发送数据失败之后能够通过AODV算法获取数
 据发送的下一跳。

 - [6]对于路由设备，直接使用CLuster 0x0007来发送到Coordinator 的心跳

 - [7]增加对于非法信道27 、28信道的支持

 - [8]增加路由设备从关联表中清除一个EndDevice设备之后上报一个ES数据,正常出货时没
 有此功能。

Bug Fixes:
 - [1] 解决设备不能退网的问题。

 - [2] 解决设备未入网时按键2次请求信号值可用的问题。

 - [3] 禁止设备退网命令ES：XXXXBB00发出后还能够接受或者发送命令。

 - [4] 解决路由设备允许加网时网关sys指示灯不对应的问题。
Known Issues:
 - [1] EndDevice 设备在加入到一个网络中，Coordinator设备在同一信道重新建网后，
 EndDevice会跟随Coordinator设备Rejion到新的PanId的问题

 - [2] 设备Resume时会直接Resume到原来的父设备下，在极限条件下设备可能出现频繁的Resume
 成功，然后又重新丢失网络，这样会增大EndDevice设备的功耗

 - [3]通信正常，突然resume的原因,可能是没有收到MAC的应答导致设备的复位

 - [4]路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

Version 1.0.4
January 08, 2016

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
 - [1] 将设备启动后网络启动延时增加到2500ms。

 - [2] 进一步完善产测机制，增加产测禁止标志到NV,产测通之后通过命令C/UUbXXXX1写产
 测禁止标志到NV中，之后不管用户是否保持SET键按下入网都不会再进入产测程序。

 - [3] 关掉ZDApp.c文件中ZDOInitDevice()函数中的 ZDApp_LeaveCtrlInit();离网控制逻
 辑可能解决设备加网后被Coordinator设备Remove的问题，同时关掉路由设备加网时的信任
 中心允许问题。

 - [4] 关闭MTO失败后重新发送MTO的问题可能解决网络的路由问题。

 - [5] 更改Router设备的Link status 为15s/次

 - [6]关闭设备调频 ZIGBEE_FREQ_AGILITY ，开启设备分帧传输功能 ，关闭NIB多播

 - [7]EndDevice设备每1h指定一次到其父设备的心跳要有APS ack否则EndDevice 设备
 Resume网络。

 - [8]增加单独按键多击功能

 - [9]将设备入网成功后SYS指示灯常亮2S移除wzcl.c文件，用户需要在应用中的
 UserAppJoinedNetworkCallBack()函数中自行调用  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
 来实现

Bug Fixes:
 - [1] 解决按键检测功能中多击后不释放造成的下次按键多击检测出错的问题。

 - [2] 禁止设备接收到网络Leave Reg命令后local device 接受该命令，造成的信任中心
 未允许设备加网的情况下设备加入到路由中被信任中心Remove出网络的问题。

Known Issues:
 - [1] EndDevice 设备在加入到一个网络中，Coordinator设备在同一信道重新建网后，
 EndDevice会跟随Coordinator设备Rejion到新的PanId的问题

 - [2] 没有同251网关一样，上电后6min内15s一次 MTO快速建立路由路径。后续考虑是否相同？

 - [3] 获取263路由的关联表时，该263路由会存在网络中不存在的短地址，该现象的原因
 是263的路由没有清除关联信息导致

 - [3] 2531 USBDongle软复位之后 USB 串口不能使用的问题？

 - [4]263的应用信息或者是心跳之后，1秒之后为POLL，然后才紧跟APS ACK而251的设备其
 紧跟APS ACK之后，1秒之后才是POLL。

 - [5]通信正常，突然resume的原因

 - [6]路由会出现绿灯一直常亮的情况，该现象的原因还没有定位。

Version 1.0.3
December 29, 2015

Notices:
 - [1]提交更新了新的版本的基础库

Changes:
 - [1]增加了完整的产测方案的支持，已经在门磁设备上初步调试通过，V1.0.2版本的应用
 需要根据V1.0.3版本的SampleDevice示例工程调整文件XXXXUserApp.c文件中的Global Variable

 - [2]改进了信号值请求机制，针对263a增加了新的信号值请求命令/C/UUeXXXX，同时保留251的信号值
 请求命令，用来和之前的版本信号值请求进行兼容。

Bug Fixes:
 - [1]

Known Issues:
 - [1]

Version 1.0.2
December 22, 2015

Notices:
 - [1]增加了一些功能的支持，解决了一个重要的BUG，更新了一些功能。

Changes:
 - [1]增加了正对灯具和内嵌式开关类产品中/C/UULXXX命令的额外指示功能的支持。
 - [2]更新了设备基本信息的上报机制，设备首次加网时上报RS、OS、固件基本信息等等
 已经加入到网络中的设备掉电复位，软复位，网络复位时不再发送OS以及ES信息，同时也
 不再开启网络Permit Join。

Bug Fixes:
 - [1]解决Router发送命令收不到Mac Ack时不断重启问题

Known Issues:
 - [1]

Version 1.0.1
December 18, 2015

Notices:
 - [1]修正了命令退网和按键退网是指示灯闪烁频率

Bug Fixes:
 - [1]修复Router 设备允许入网时，设备不能加入到网络的问题。

Known Issues:
 - [1]Router设备在Coordinator设备断电时，由于发送到Coordinator数据没有应答导致Router
网络不断重启。

Version 1.0.0
December 10, 2015

Notices:
 - [1]协议栈基础库版本全面升级到V1.0.0版本，本版是正式发布的第一个版本，本版本综
 合了之前发布的两个测试版本的不足之处，同时增加了对新型开关处理方案的支持。

 - [2]本版本增加了产测方案的支持。本产测方案是全新的产测方案，需配合全新的2.6.3a
 网关来实现产品生产测试，不同于之前的产测方案，不可混用。

Changes:
 - [1]变更绑定函数移除函数内部指示灯闪烁函数，开发者如需要绑定时闪烁LED可使用函数
 HalLedBlink ( HAL_LED_3, 1, 50, 500 );只需要指定需要闪烁的LED灯即可;同时对应的
 绑定函数接口变为
 void WZCL_InBindingReqMessage(uint8 endpoint ,cId_t* bindingInClusters,uint8 clusterlists);
 void WZCL_OutBindingReqMessage(uint8 endpoint ,cId_t* bindingOutClusters,uint8 clusterlists);
 开发者需要在调用绑定和闪灯函数之前判定一下设备是否在网，可通过if((devState ==
 DEV_END_DEVICE) || (devState == DEV_ROUTER))来判定。

 - [2]增加允许加网函数接口void WZCL_PermitJoin(uint8 period);

 - [3]增加获取多击按键次数接口uint8 WZCL_ReadKeyMultiClicks(void)此函数仅仅能够在函数
 void UserAppHandleKeys( uint8 shift, uint8 keys )内部调用;

 - [4]增加获取按键是否长按接口uint8 WZCL_ReadKeyHoldPress(void)此函数仅仅能够在函数
 void UserAppHandleKeys( uint8 shift, uint8 keys )内部调用;

 - [5]增加获取信号值接口void WZCL_InterRssiReqMessage(void)可通过调用该函数获取
 设备的上下行信号值并且自动发送到网关;同时增加了引用函数
 void UserAppRssiBlinkLed(wzclDevRssiRsp_t *pRsp)开发者可将设备已经获取的信号值
 强弱通过指示灯的闪烁的形式表现出来。

 - [6]增加设备加网接口void WZCL_FindAndJoinNetwork(void)开发者可在适当的时候直接调用
 该函数进行加网操作，另外设备上电自动加网可直接调用函数
 ZDOInitDevice(0x00~0xFFFE);来实现

 - [7]增加设备离网接口void WZCL_LeaveAndRestoreFactory(void)开发者可在适当的时候直接调用
 该函数进行离网操作，如果需要另外的指示灯来指示设备离网可在调用该函数前调用闪灯函数指示，
 有效利用时间为4.5s钟

 - [8]增加设备开始加网回调函数void UserAppStartNwkJoiningCallBack(void)设备开始加
 网时会调用该函数，如开发者需要在此时实现一些操作可在此引用函数内进行。

 - [9]增加加网闪灯回调函数，如需要在每次加网时闪烁一下LED灯来指示，可在函数
 void inline UserAppJoinNetworkBlinkCallback(void)内通过闪灯来表现，一般调用
   HalLedBlink ( HAL_LED_1, 1, 30, 500 )即可，特殊情况下用户可适当调整参数。

Bug Fixes:
 - [1]
 - [2]

Known Issues:
 - [1]


Version 0.1.1
November 24, 2015

Notices:
 - [1]
Changes:
 - [1]增加对场景开关设备的支持。
 - [2]增加发送绑定消息的函数WZCL_SendBindingMessage。
 - [3]变更信号检测ClusterId为0x000A,ClusterId 0x000B变更为Dimmer ClusterId。
 - [4]增加对温室度等具有周期性上报数据的设备的周期性上报功能UserAppProcessReportingCommand的支持。
 - [5]增加需要有应答返回的DS数据发送函数void WZCL_SendDsWithAckMessage(uint8 * iMsg, uint8 len)。
 - [6]变更绑定函数增加指示LED的参数接口
 void WZCL_InBindingReqMessage(uint8 endpoint ,cId_t* bindingInClusters,uint8 clusterlists,uint8 leds);
 void WZCL_OutBindingReqMessage(uint8 endpoint ,cId_t* bindingOutClusters,uint8 clusterlists,uint8 leds);。
Bug Fixes:
 - [1]
 - [2]

Known Issues:
 - [1]