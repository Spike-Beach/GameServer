#pragma once

#include "SBUtils.h"

#define WAIT_SEC 3000
#define ROUND_COUNT_DOWN_SEC 5

enum TeamKind : bool
{
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
	GameStatus Status();
	void Clear();
	bool SetGame(INT32 gameId, Team redTeam, Team blueTeam);
	bool UserIn(SBUser* UserId);
	bool UserOut(SBUser* UserId);
	bool PlayingSync();
	bool WaitUserSync();
	bool LeaveSync();
private:
	INT32 _gameId;
	//std::mutex _gameMutex;
	std::shared_mutex _gameMutex;
	std::atomic<GameStatus> _gameStatus;
	// INT64:userId, SBUser*:user
	std::unordered_map<INT64, SBUser*> _redTeam;
	std::unordered_map<INT64, SBUser*> _blueTeam;
	std::chrono::system_clock::time_point _gameStartTime;
	//std::chrono::system_clock::time_point _lastSyncTime;
	std::chrono::system_clock::time_point _roundStartTime;
	std::chrono::system_clock::time_point _waitDeadLine;

	Ball _ball;
	INT16 _redScore;
	INT16 _blueScore;
	std::pair<TeamKind, SBUser*> _leaveUser;

	void NoticeInGame(std::vector<char>&& notify);
	bool Score(SyncResult scoreResult);
	void RedWin();
	void BlueWin();
	std::vector<char> GenGameSyncData();
};

