#pragma once
struct UserInfo
{
	INT64 id = -1;
	std::string nickName;
};
using Team = std::array<UserInfo, 2>;

enum class GameStatus : UINT16
{
	EMPTY = 0,
	WAITING = 1,
	PLAYING = 2,
	FINISHING = 3,
};

enum class SyncResult : UINT16
{
	NONE = 0,
	RED_WIN = 1,
	BLUE_WIN = 2,
};

class GameObj
{
	// 위치 정보 및 속도/가속도 등등..
public:
	SyncResult Sync()
	{
		return SyncResult::NONE;
	};
	void Reset() {};
};

class SBUser : public GameObj
{
public:
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
	INT32 GetGameId() { return _gameId; }

	friend class SBUserManager;
	SBUser() :_id(-1), _sessionId(-1), _token(""), _assignedId(""), _nickName("") {}
private:
	INT64 _id;
	INT64 _sessionId; // Only SBUserManager write
	std::string _token;
	std::string _assignedId;
	INT32 _gameId;
	std::string _nickName;
};

template <typename T>
class ThreadJobQueue
{
public:
	void Push(T data)
	{
		std::unique_lock<std::mutex> lock(_inputMutex);
		_inputQueue->push(data);
	}

	std::optional<T> Pop()
	{
		if (_outputQueue->empty())
		{
			std::unique_lock<std::mutex> lock(_inputMutex);
			std::swap(_inputQueue, _outputQueue);
		}
		if (_outputQueue->empty())
		{
			return nullptr;
		}
		T output = _outputQueue->front();
		_outputQueue->pop();
		return output;
	}

private:
	std::mutex _inputMutex;
	std::queue<T> _queueA;
	std::queue<T> _queueB;
	std::queue<T>* _inputQueue;
	std::queue<T>* _outputQueue;
};