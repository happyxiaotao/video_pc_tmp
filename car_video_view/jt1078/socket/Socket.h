#ifndef SWORD_SOCKET_H
#define SWORD_SOCKET_H
#include <string>

#ifdef WIN32
#include <WinSock2.h>
#endif

namespace sock {

inline uint16_t networkToHost16(uint16_t value)
{
    return ntohs(value);
}

inline uint32_t networkToHost32(uint32_t value)
{
    return ntohl(value);
}

inline uint64_t networkToHost64(uint64_t value)
{
#ifdef WIN32
    return ntohll(value);
#else
#if __BYTE_ORDER == __LITTLE_ENDIANkk
    return (((uint64_t)ntohl(value)) << 32 | ntohl(value >> 32));
#else
    return value;
#endif
#endif
}

} // namespace socket

#endif // SWORD_SOCKET_H
