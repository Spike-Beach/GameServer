#pragma once

//#include <WinSock2.h>
//#include <array>
//#include <vector>
//#include <optional>
#include "Packet.h"

#define BUFSIZE 1024

enum IO_TYPE
{
    READ, WRITE, NONE
};

class IocpData
{
public:
    IocpData();
    ~IocpData();
    IO_TYPE GetIoType();
    WSABUF GetWSABuf();
    //WSABUF GetWSABuf();
    OVERLAPPED* GetOverlappedPtr();
    char* GetBufPtr();
    void Clear();

    void SetIoType(IO_TYPE ioType);
    bool IsNeedMoreIo(size_t transferSize);

    // for recv
    std::optional<Packet> ReadPacketFromBuf();
    // for send
    bool WritePacketToBuf(Packet packet);
private:   
    OVERLAPPED _overlapped;
    IO_TYPE _ioType;
    std::array<char, BUFSIZE> _buffer;
    //std::vector<char> _buffer;
    size_t _desiredLength; // Packet에 기입된 길이.
    size_t _currentLength; // GetQueuedCompletionStatus로 받은 길이.
    
    bool SetdesiredLength();
};

