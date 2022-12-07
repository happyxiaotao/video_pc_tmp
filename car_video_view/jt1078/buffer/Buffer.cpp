
#include "Buffer.h"
#include "../socket/Socket.h"
#include <assert.h>

Buffer::Buffer()
    : m_recv_buffer(nullptr)
    , m_read_index(0)
{
    m_recv_buffer = new char[MAX_SIZE_ONCE_READ + 1];
    m_recv_buffer[0] = '\0';
}
Buffer::Buffer(const Buffer& buffer)
    : m_buffer(buffer.GetBuffer(), buffer.ReadableBytes())
    , m_read_index(0)
{
    m_recv_buffer = new char[MAX_SIZE_ONCE_READ + 1];
    m_recv_buffer[0] = '\0';
}
Buffer::Buffer(Buffer&& buffer)
    : m_buffer(buffer.GetBuffer(), buffer.ReadableBytes())
    , m_read_index(0)
{
    m_recv_buffer = new char[MAX_SIZE_ONCE_READ + 1];
    m_recv_buffer[0] = '\0';
    std::string str;
    buffer.m_buffer.swap(str);
    buffer.m_read_index = 0;
}
Buffer::~Buffer()
{
    if (m_recv_buffer != nullptr) {
        delete[] m_recv_buffer;
        m_recv_buffer = nullptr;
    }
}
void Buffer::Clear()
{
    m_buffer.clear();
    m_read_index = 0;
}

void Buffer::Skip(size_t n)
{
    m_read_index += n;
}

int8_t Buffer::ReadInt8()
{
    return ReadInterger<int8_t>();
}
int16_t Buffer::ReadInt16()
{
    // return sock::networkToHost16(ReadInterger<int16_t>());
    return ReadInterger<int16_t>();
}
int32_t Buffer::ReadInt32()
{
    // return sock::networkToHost32(ReadInterger<int32_t>());
    return ReadInterger<int32_t>();
}
int64_t Buffer::ReadInt64()
{
    // return sock::networkToHost64(ReadInterger<int64_t>());
    return ReadInterger<int64_t>();
}
void Buffer::ReadBuffer(char* dst, size_t n)
{
    assert(ReadableBytes() >= n);
    memcpy(dst, GetBuffer(), n);
    Skip(n);
}
