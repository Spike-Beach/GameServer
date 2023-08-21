#pragma once
#include "SBUser.h"
#include "SBUtils.h"

#define WAIT_SEC 3000
#define ROUND_COUNT_DOWN_SEC 5

enum class TeamKind : INT16
{
	EMPTY,
	RED,
	BLUE
};

class Map
{

};



class Ball : public GameObj
{

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
	
	bool Controll(INT64 userId, INT64 ctlTime, Acceleration acc, Acceleration stopAcc);
	bool PlayingSync();
	bool WaitUserSync();
	bool LeaveSync();
	
	void NoticeInGame(std::vector<char>&& notify);
private:
	INT32 _gameId;
	std::chrono::system_clock::time_point _waitDeadLine;

	std::shared_mutex _gameMutex;
	std::atomic<GameStatus> _gameStatus;

	/*std::unordered_map<INT64, SBUser*> _redTeam;
	std::unordered_map<INT64, SBUser*> _blueTeam;*/
	std::map<INT64, SBUser* > _redTeam;
	std::map<INT64, SBUser* > _blueTeam;
	std::chrono::system_clock::time_point _gameStartTime;
	std::chrono::system_clock::time_point _roundStartTime;

	Ball _ball;
	INT16 _redScore;
	INT16 _blueScore;
	std::pair<TeamKind, SBUser*> _leaveUser;
	std::chrono::system_clock::time_point _lastSyncTime; // ��Ŷ�� synctime�� _lastSyncTime���� �����̸� ���� ��Ŷ�� ����� ����.
	//SyncReq _syncPacket;
	std::shared_mutex _syncPacketMutex;
	std::chrono::system_clock::time_point _packetGenTime;
	std::vector<char> _serializedSyncPacket;

	bool Score(SyncResult scoreResult);
	void RedWin();
	void BlueWin();
	INT16 FindUser(INT64 userId, SBUser** userPtr);
	std::vector<char> GenSerializedSyncPacket();
};

