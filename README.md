
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
