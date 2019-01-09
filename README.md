# cc2530
数据帧操作流程：设备---zigbee---APP
设备上报：设备上报三路至zigbee，zigbee再到APP
APP下发：APP下发一路到zigbee,zigbee再到设备；此时设备应答一路上报

ZBC软件/APP软件发送指令，十六进制格式最后会追加两个固定字节 0D 0A,提取前三个有效字节。
设备上传指令，若获取了十进制数：
将十进制数转换成字符串
sprintf(DecToString, "%d", ExchangeData);
再将字符串转换成十六进制数
StringToHex = strtol(DecToString, 0, 16);
例：054D---“054”----0x054 (此时的54不是0x36) 


APP每次重开，都下发100。Zigbee实现保存上次的数值程序设计思路：
每次APP下发指令时，面板都会回复一条指令。思路的关键之处在于，APP下发000和面板上传关000，
可以设置标志位F=1；APP下发亮度值和面板上传其他亮度值时，可以设置F=2,并保存此时的亮度值。
此方案可以有效的判断出是APP打开是发的100，还是调光时发的100。
      if(ExchangeData == 0)
      {
        RetainNwk[0] = 1; 
      }
      else 
      {
        RetainNwk[0] = 2; //实现调到一百
        RetainNwk[1] = ExchangeData;
      }

      //APP开状态和调到100的区别
      if(ExchangeData == 100)
      {
        //APP引起的OFF和设备引起的OFF的区别
        if(RetainNwk[0] == 1)
        {
          RetainNwk[0] = 0;
          ExchangeData = RetainNwk[1];
        }
      }
      if(ExchangeData != 0)
        RetainNwk[2] = ExchangeData; //保留当前app下发的数据
 

十进制000-100映射十六进制000-254（1-100对1-254）
十进制到十六进制
  //偶数 value/2*2  +  (value/2-1)*3  +  2
  //奇数 value/2*2  +  (value/2  )*3  +  2
十六进制到十进制
//将1-100每两个数看出一个数，对应形成50个数组，每个数组中包含5个数。先取出对应的数组下标值和单个数组内的对应值，然后计算出十进制的值。
（可查看Excel十进制对十六进制映射关系表）



面板上传返回三路(串口115200)
17个字节    FA 0E 14 02 FF FF FF FF 02 03 01 00 02 00 03 00 25
起始位FA 
包长10字节0E（3路）14 02 FF FF FF FF 02 03 01 亮度 02 亮度 03 亮度 
校验和

APP控制下发一路，此次面板回发一路，13个字节
FA 0A 12 02 FF FF FF FF 02 01 01 00 18
（返回14，发送12）

退网申请，电源键连续短按4次，第5次长按3S以上，15个字节
在快速点击电源的时候，亮度值会减小
FA 0E 14 02 FF FF FF FF 02 03 01 01 02 00 03 00 26 
FA 0E 14 02 FF FF FF FF 02 03 01 00 02 00 03 00 25 
FA 0E 14 02 FF FF FF FF 02 03 01 01 02 00 03 00 26 
FA 0E 14 02 FF FF FF FF 02 03 01 00 02 00 03 00 25 
FA 0E 14 02 FF FF FF FF 02 03 01 01 02 00 03 00 26 
FA 0C 12 24 FF FF FF FF 02 FF FF FF FF FF 35
（退网反馈，回发灯闪指令FA 08 12 33 00 00 FF FE FF FE 41，发一次亮一次

FA 08 12 33 00 00 FF FE FF FE 41




