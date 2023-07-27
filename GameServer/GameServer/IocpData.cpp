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

std::optional<Packet> IocpData::ReadPacketFromBuf()
{
	if (_desiredLength > _currentLength || _currentLength > _buffer.size())
	{
		// TODO : log
		return std::nullopt;
	}

	Packet packet;
	if (packet.Deserialize(_buffer.data(), _desiredLength) == false)
	{
		// TODO : log
		return std::nullopt;
	}
	
	size_t remainingDataSize = _currentLength - _desiredLength;
	std::memmove(_buffer.data(), _buffer.data() + _desiredLength, remainingDataSize);
	
	_currentLength = remainingDataSize;
	_desiredLength = 0;
	
	return packet;
}

bool IocpData::WritePacketToBuf(Packet packet)
{
	std::vector<char> serializedPacket = packet.Serialize();
	std::copy(serializedPacket.begin(), serializedPacket.end(), _buffer.begin());
	_desiredLength = serializedPacket.size();
	return true;
}