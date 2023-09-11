#include "pch.h"
#include "IocpData.h"

// 목표하는 길이와, 현재 받은 길이를 비교하여 WSABUF의 시작위치와 길이를 반환.
WSABUF IocpData::GetWSABuf()
{
	//WSABUF wsabuf = { 0, nullptr };
	//if (_desiredLength == 0 && _currentLength == 0)
	//{
	//	wsabuf.buf = _buffer.data();
	//	wsabuf.len = _buffer.size();
	//}
	//else if (_desiredLength != 0)
	//{
	//	wsabuf.len = _desiredLength - _currentLength;
	//	if (wsabuf.len <= BUFSIZE)
	//	{
	//		wsabuf.buf = _buffer.data() + _currentLength;
	//	}
	//}
	//else // _desiredLength == 0 < _currentLength != 0
	//{
	//	g_logger.Log(LogLevel::ERR, "IocpData::GetWSABuf()", "Invalid _desiredLength length. \
	//		_desiredLength : " + std::to_string(_desiredLength) + ", _currentLength : " + std::to_string(_currentLength));
	//	return wsabuf;
	//}
	WSABUF wsabuf;
	if (_currentLength >= _buffer.size())
	{
		wsabuf = { 0, nullptr };
	}
	else
	{
		wsabuf = { static_cast<ULONG>(_buffer.size() - _currentLength), _buffer.data() + _currentLength };
	}
	return wsabuf;
}

// recv, send시 io작업이 더 필요한지 체크하는 함수.
bool IocpData::IsNeedMoreIo(size_t transferSize)
{
	_currentLength += transferSize;
	if (_ioType == IO_TYPE::READ && _desiredLength == 0)
	{
		SetdesiredLength();
	}

	if (_currentLength < _desiredLength)
	{
		return true;
	}
	return false;
}

bool IocpData::SetdesiredLength()
{
	size_t parsedPacketLength = Packet::ParsePacketLength(_buffer.data(), _currentLength);
	if (parsedPacketLength != 0)
	{
		_desiredLength = parsedPacketLength;
		return true;
	}
	return false;
}

Package IocpData::ReadPacketFromBuf()
{
	Package package;
	if (_desiredLength > _currentLength || _currentLength > _buffer.size())
	{
		g_logger.Log(LogLevel::ERR, "IocpData::ReadPacketFromBuf", "Invalid length. \
			_desiredLength : " + std::to_string(_desiredLength)
			+ ", _currentLength : " + std::to_string(_currentLength)
			+ ", _buffer.size() : " + std::to_string(_buffer.size()));
		package.pakcetId = PacketId::ERROR_OCCUR;
		return package;
	}

	// 패키지 생성 <- pakcetId, sessionId, buffer 정보 포함
	package._buffer.resize(_desiredLength);
	std::memmove(package._buffer.data(), _buffer.data(), _desiredLength);
	package.pakcetId = Packet::GetPacketId(package._buffer.data(), package._buffer.size());
	
	// 남은 데이터를 버퍼의 시작부분으로 땡겨옴.
	size_t remainingDataSize = _currentLength - _desiredLength;
	std::memmove(_buffer.data(), _buffer.data() + _desiredLength, remainingDataSize);
	_currentLength = remainingDataSize;
	_desiredLength = 0;
	
	return package;
}

bool IocpData::WriteToBuf(std::vector<char> serializedPacket)
{
	std::copy(serializedPacket.begin(), serializedPacket.end(), _buffer.begin());
	_desiredLength = serializedPacket.size();
	_currentLength = 0;
	return true;
}

IocpData::IocpData()
: _desiredLength(0), _currentLength(0), _ioType(IO_TYPE::NONE)
{
	ZeroMemory(&_overlapped, sizeof(_overlapped));
}

IocpData::~IocpData()
{
}

void IocpData::Clear()
{
	ZeroMemory(&_overlapped, sizeof(_overlapped));
	_desiredLength = 0;
	_currentLength = 0;
}

void IocpData::SetIoType(IO_TYPE ioType)
{
	_ioType = ioType;
}

IO_TYPE IocpData::GetIoType()
{
	return _ioType;
}

OVERLAPPED* IocpData::GetOverlappedPtr()
{
	return &_overlapped;
}

char* IocpData::GetBufPtr()
{
	return _buffer.data();
}