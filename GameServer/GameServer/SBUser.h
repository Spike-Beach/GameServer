#pragma once
#include "GameObj.h"

class SBUser : public GameObj
{
public:
	SBUser() :_id(-1), _sessionId(-1), _token(""), _assignedId(""), _nickName("") {}
	SBUser(const SBUser& other) : _id(other._id), _sessionId(other._sessionId), _token(other._token), _assignedId(other._assignedId), _nickName(other._nickName) {}
	bool CheckToken(std::string requredToken)
	{
		return _token == requredToken;
	}

	void SetUser(const std::string& assignedId, const std::string& token, INT32 gameId)
	{
		//_id = id;
		_assignedId = assignedId;
		_token = token;
		_gameId = gameId;
	}

	void SetUserId(INT64 id)
	{
		_id = id;
	}

	void SetUserNickname(const std::string& nickname)
	{
		_nickName = nickname;
	}

	void Clear()
	{
		_id = -1;
		_sessionId = -1;
		_token.clear();
		_assignedId.clear();
	}

	INT64 GetId() { return _id; }
	INT64 GetSessionId() { return _sessionId; }
	std::string GetToken() { return _token; }
	std::string GetAssignedId() { return _assignedId; }
	std::string GetNickName() { return _nickName; }
	INT32 GetGameId() { return _gameId; }

	friend class SBUserManager;
private:
	INT64 _id;
	INT64 _sessionId; // Only SBUserManager write

	std::string _token;
	std::string _assignedId;
	INT32 _gameId;
	std::string _nickName;
};
