#pragma once

//#include "Packet.h"
#include "IocpData.h"
#include "Session.h"

#define IO_KIND_MAX 2


class IocpSession : public Session
{
public:
    IocpSession(INT16 sessionId, SOCK_TYPE sessionType);
    ~IocpSession();
    void Close();
    void Clear();

    INT32 GetId();

    void KeepSendData(size_t transferSize);
    std::optional<Package> KeepRecvPacket(size_t transferSize);
    
    void SendData(std::vector<char>&& serializedPacket);
  
    // use to trig
    void RecvTrigger();
private:
    std::array<IocpData, IO_KIND_MAX> _iocpData;
};