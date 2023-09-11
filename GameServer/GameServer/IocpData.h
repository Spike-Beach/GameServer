#pragma once
#include "Package.h"

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
    OVERLAPPED* GetOverlappedPtr();
    char* GetBufPtr();
    void Clear();
    void SetIoType(IO_TYPE ioType);

    bool IsNeedMoreIo(size_t transferSize); // keep io or not
    Package ReadPacketFromBuf(); // for recv
    bool WriteToBuf(std::vector<char> serializedPacket); // for send
private:   
    OVERLAPPED _overlapped;
    IO_TYPE _ioType;
    std::array<char, BUFSIZE> _buffer;
    size_t _desiredLength; // Packet에 기입된 길이.
    size_t _currentLength; // GetQueuedCompletionStatus로 받은 길이.
    
    bool SetdesiredLength();
};

