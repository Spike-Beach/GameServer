#pragma once

#include "IocpData.h"
#include "Session.h"

class IocpSession : public Session
{
public:
    ~IocpSession();
    INT32 GetId();

    void KeepSendData(size_t transferSize);
    std::optional<Package> KeepRecvPacket(size_t transferSize);
    void SendData(std::vector<char> serializedPacket);
    void RecvTrigger(); // recv트리거

private:
    // SessionManager만이 생성자 호출 및 정리 할 수 있게 함.
    friend class SessionManager;
    IocpSession(INT16 sessionId, SOCK_TYPE sessionType);
    void Close();
    void Clear();

    std::array<IocpData, 2> _iocpData; // 2: io타입. read or wirte
};