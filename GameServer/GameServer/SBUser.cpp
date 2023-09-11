#include "pch.h"
#include "SBUser.h"

bool SBUser::CheckToken(std::string requredToken)
{
	return _token == requredToken;
}

void SBUser::SetUser(const std::string& assignedId, const std::string& token, INT32 gameId)
{
	_assignedId = assignedId;
	_token = token;
	_gameId = gameId;
}

void SBUser::SetUserId(INT64 id)
{
	_id = id;
}

void SBUser::SetUserNickname(const std::string& nickname)
{
	_nickName = nickname;
}

void SBUser::Clear()
{
	_id = -1;
	_sessionId = -1;
	_token.clear();
	_assignedId.clear();
}

INT64 SBUser::GetId() 
{ 
	return _id; 
}

INT64 SBUser::GetSessionId() 
{ 
	return _sessionId; 
}

std::string SBUser::GetToken() 
{ 
	return _token; 
}

std::string SBUser::GetAssignedId() 
{ 
	return _assignedId; 
}

std::string SBUser::GetNickName() 
{ 
	return _nickName; 
}

INT32 SBUser::GetGameId() 
{ 
	return _gameId; 
}

SBUser::SBUser() 
	:_id(-1), _sessionId(-1), _token(""), _assignedId(""), _nickName("") 
{
}

SBUser::SBUser(const SBUser& other) 
	: _id(other._id), _sessionId(other._sessionId), _token(other._token), 
	_assignedId(other._assignedId), _nickName(other._nickName) 
{
}