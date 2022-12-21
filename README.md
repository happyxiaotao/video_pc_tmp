
### 相关库的使用
#### 使用ffmpeg库
从官网的下载入口：https://ffmpeg.org/download.html
进入到此入口：https://github.com/BtbN/FFmpeg-Builds/releases
下载的地址是：https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl-shared.zip
目前使用到的库：-lavcodec -lavutil -lswscale
avcodec-59.dll
avutil-57.dll
swscale-6.dll


#### 使用的SSL相关库
目前使用到的库：
目前在D:\Qt\Tools\OpenSSL\Win_x64\bin下面有Qt已经编译好的，可以使用
libssl-1_1-x64.dll
libcrypto-1_1-x64.dll


### API接口说明
账号登录接口： https://tg808.gcpgps.com/v1/public/system/UserLogin  POST请求
实时视频树获取接口：https://tg808.gcpgps.com/v1/impower/carinfo/GetNewVideoChannelTree  GET请求
实时视频车辆定位接口：https://tg808.gcpgps.com/v1/impower/car808/PositionAndDecode


### 实时视频

##### 车队列表状态
目前有十一个状态：
实时视频树获取接口GetNewVideoChannelTree返回的status字段，表示车辆的状态。
目前如下：

 

| status字段值 | 含义                 |
| :----------: | :------------------- |
|      0       | 离线                 |
|      1       | 离线-从未报定位      |
|      2       | 离线报警             |
|      3       | 在线-从未报定位      |
|      4       | 在线-行驶            |
|      5       | 在线-行驶-报警       |
|      6       | 在线-停车-ACC关      |
|      7       | 在线-停车-ACC开      |
|      8       | 在线-停车-ACC关-报警 |
|      9       | 在线-停车-ACC开-报警 |
|      10      | 在线-无效定位        |
|      11      | 欠费                 |

对应图片如下：

![](.\doc\车辆状态.png)

