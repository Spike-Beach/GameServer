#pragma once
#include "GameObj.h"

class SBUser : public GameObj
{
public:
	SBUser(const SBUser& other);
	void Clear();
	bool CheckToken(std::string requredToken);

	INT64 GetId();
	INT64 GetSessionId();
	std::string GetToken();
	std::string GetAssignedId();
	std::string GetNickName();
	INT32 GetGameId();
	
	void SetUser(const std::string& assignedId, const std::string& token, INT32 gameId);
	void SetUserId(INT64 id);
	void SetUserNickname(const std::string& nickname);

private:
	// friend�� �̿��ؼ� SBUserManager���� ���� �� ���� �Ҵ��� �� �ְ� ��.
	friend class SBUserManager;
	SBUser();
	INT64 _id;
	INT64 _sessionId;

	std::string _token;
	std::string _assignedId;
	INT32 _gameId;
	std::string _nickName;
};
