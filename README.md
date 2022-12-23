
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

### 打包
参考网站：
https://www.cnblogs.com/linuxAndMcu/p/10974927.html#:~:text=Qt%20%E9%A1%B9%E7%9B%AE%E5%BC%80%E5%8F%91%E5%AE%8C%E6%88%90%E4%B9%8B%E5%90%8E%EF%BC%8C%E9%9C%80%E8%A6%81%E6%89%93%E5%8C%85%E5%8F%91%E5%B8%83%E7%A8%8B%E5%BA%8F%EF%BC%8C%20%E8%80%8C%E5%9B%A0%E4%B8%BA%E7%94%A8%E6%88%B7%E7%94%B5%E8%84%91%E4%B8%8A%E6%B2%A1%E6%9C%89%20Qt%20%E9%85%8D%E7%BD%AE%E7%8E%AF%E5%A2%83%EF%BC%8C%E6%89%80%E4%BB%A5%E9%9C%80%E8%A6%81%E5%B0%86%20release%20%E7%94%9F%E6%88%90%E7%9A%84%20exe,dll%20%E6%96%87%E4%BB%B6%E5%A4%8D%E5%88%B6%E5%88%B0%E4%B8%80%E4%B8%AA%E6%96%87%E4%BB%B6%E5%A4%B9%E4%B8%AD%EF%BC%8C%E7%84%B6%E5%90%8E%E5%86%8D%E7%94%A8%20Inno%20Setup%20%E6%89%93%E5%8C%85%E5%B7%A5%E5%85%B7%E6%89%93%E5%8C%85%E6%88%90%E4%B8%80%E4%B8%AA%20exe%20%E5%AE%89%E8%A3%85%E5%8C%85%EF%BC%8C%20%E5%B0%B1%E5%8F%AF%E4%BB%A5%E5%8F%91%E5%B8%83%E4%BA%86%E3%80%82


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

