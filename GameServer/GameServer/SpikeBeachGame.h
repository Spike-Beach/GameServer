#pragma once
#include "SBUser.h"
#include "SBUtils.h"
#include "Ball.h"

#define WAIT_SEC 30
#define ROUND_COUNT_DOWN_SEC 5

enum class SyncResult : INT16
{
	ERR = -1,
	NONE = 0,
	GAMEFIN = 1,
	TIMEOVER = 4,
	SOMEONELEAVE = 5
};

class SpikeBeachGame
{
public:
	SpikeBeachGame();
	SpikeBeachGame(const SpikeBeachGame&) {};
	GameStatus GetStatus();
	void Clear();
	void ResetToNewGame();
	std::vector<char> GetSerialiedSyncPacket();
	bool SetGame(INT32 gameId, Team redTeam, Team blueTeam);
	bool UserIn(SBUser* UserId);
	bool UserOut(SBUser* UserId);
	
	bool Controll(INT64 userId, INT64 ctlTime, Acceleration acc);
	bool UpdateLatency(INT64 userId, INT64 clientTime);

	SyncResult PlayingSync();
	SyncResult WaitUserSync();
	SyncResult LeaveSync();
	
	void NoticeInGame(std::vector<char>&& notify);
private:
	INT32 _gameId;
	std::chrono::system_clock::time_point _waitDeadLine;

	std::shared_mutex _gameMutex;
	std::atomic<GameStatus> _gameStatus;

	// 0 : red1, 1 : red2, 2 : blue1, 3 : blue2
	std::array<std::pair<INT64, SBUser* >, 4> _users;
	std::chrono::system_clock::time_point _gameStartTime;
	std::chrono::system_clock::time_point _roundStartTime;
	
	// 패킷의 synctime이 _packetGenTime보다 과거이면 새로 패킷을 만들고 갱신.
	std::shared_mutex _syncPacketMutex;
	std::chrono::system_clock::time_point _packetGenTime;
	std::vector<char> _serializedSyncPacket;

	Ball _ball;
	INT16 _redScore;
	INT16 _blueScore;
	size_t _leaveUserIdx;
	std::chrono::system_clock::time_point _lastSyncTime; 

	SyncResult Score(BallResult scoreResult);
	void RedWin();
	void BlueWin();
	INT16 FindUser(INT64 userId, SBUser** userPtr);
	std::vector<char> GenSerializedSyncPacket();
};

