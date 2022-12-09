#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#include <QString>
namespace common {
namespace const_str {
#define SETTING "setting"
#define DEVS "devs"
#define DEVS "devs"
#define UUID "uuid"
#define NAME "name"
#define UPDATE_TIME "update_time"
#define PG_SVR_ADDR "svr_addr"
#define MEDIA_DIR "media_dir"
#define VIDEO "video"
#define CAREMA "carema"
}

namespace layout {

    // 布局矩阵类型
    enum eVideoLayoutType : int {
        kInvalidType = 0, //无效的类型
        kMatrix_One_One = 1, // 1*1
        kMatrix_Two_Two, // 2*2
        kMatrix_Three_Three, // 3*3
        kMatrix_Four_Four, // 4*4
    };
}

namespace pushbuttonClicked {
    enum PushButtonClickedType {
        eCloud = 1,
        eSdCard,
        eClose, // 关闭
        eCarema, // 拍照
        eSpeakStart, // 开始说话
        eSpeakStop, // 停止说话
        eRecordVideoStart, // 开始录像
        eRecordVideoStop, // 停止录像
        ePtzCtr_Up, // 云台控制 上
        ePtzCtr_Down, // 云台控制 下
        ePtzCtr_Left, // 云台控制 左
        ePtzCtr_Right, // 云台控制 右
        eAudioPlay, // 播放声音
        eAudioMute, // 静音
        // eFullScreen, // 取消全屏
        // eNormalScreen, // 全屏
        eCloseAll, //关闭全部
    };
}
};

#endif // COMMONDEFINE_H
