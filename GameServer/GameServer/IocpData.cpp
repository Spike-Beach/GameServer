#include "pch.h"
#include "IocpData.h"


IocpData::IocpData()
: _desiredLength(0), _currentLength(0), _ioType(IO_TYPE::NONE)//,
//_buffer({0})
{
	ZeroMemory(&_overlapped, sizeof(_overlapped));
}

IocpData::~IocpData()
{
	// µ¿ÀûÇÒ´çÇÑ°Å ¾øÀ¸´Ï °Á ³ÀµÖµµ µÉµí.
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

WSABUF IocpData::GetWSABuf()
{
	WSABUF wsabuf = { 0, nullptr };
	if (_desiredLength < _currentLength)
	{
		return wsabuf;
	}
	else if (_desiredLength == 0 && _currentLength == 0)
	{
		wsabuf.buf = _buffer.data();
		wsabuf.len = _buffer.size();
	}
	else if (_desiredLength != 0)
	{
		wsabuf.len = _desiredLength - _currentLength;
		if (wsabuf.len <= BUFSIZE)
		{
			wsabuf.buf = _buffer.data() + _currentLength;
		}
	}

	return wsabuf;
}

OVERLAPPED* IocpData::GetOverlappedPtr()
{
	return &_overlapped;
}

char* IocpData::GetBufPtr()
{
	return _buffer.data();
}

bool IocpData::IsNeedMoreIo(size_t transferSize)
{
	_currentLength += transferSize;
	if (_ioType == IO_TYPE::READ && _desiredLength == 0)
	{
		SetdesiredLength();
	}

	if (_currentLength < _desiredLength || _desiredLength == 0)
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
	if (_desiredLength > _currentLength 
		|| _currentLength > _buffer.size())
	{
		g_logger.Log(LogLevel::ERR, "IocpData::ReadPacketFromBuf", "Invalid length. \
			_desiredLength : " + std::to_string(_desiredLength)
			+ ", _currentLength : " + std::to_string(_currentLength)
			+ ", _buffer.size() : " + std::to_string(_buffer.size()));
		package.pakcetId = PacketId::ERROR_OCCUR;
		return package;
	}

	package._buffer.resize(_desiredLength);
	std::memmove(package._buffer.data(), _buffer.data(), _desiredLength);
	package.pakcetId = Packet::GetPacketId(package._buffer.data(), package._buffer.size());
	
	size_t remainingDataSize = _currentLength - _desiredLength;
	std::memmove(_buffer.data(), _buffer.data() + _desiredLength, remainingDataSize);
	_currentLength = remainingDataSize;
	_desiredLength = 0;
	
	return package;
}

bool IocpData::WriteToBuf(std::vector<char>&& serializedPacket)
{
	std::copy(serializedPacket.begin(), serializedPacket.end(), _buffer.begin());
	_desiredLength = serializedPacket.size();
	return true;
}