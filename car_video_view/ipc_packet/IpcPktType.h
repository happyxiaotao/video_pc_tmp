#ifndef IPC_PACKET_TYPE_H
#define IPC_PACKET_TYPE_H

#include <stdint.h>
namespace ipc {

// 一定要与服务器中的IpcPktType类型一一对应，否则会出现未知错误
enum IpcPktType : uint32_t {
    IPC_PKT_INVALID_PACKET = 0,

    IPC_PKT_HEARTBEAT,

    IPC_PKT_SUBSCRIBE_DEVICE_ID, //订阅iccid
    IPC_PKT_UNSUBSCRIBE_DEVICE_ID, //取消订阅iccid

    IPC_PKT_JT1078_PACKET, // jt1078数据包
};
} // namespace ipc

#endif // IPC_PACKET_TYPE_H
