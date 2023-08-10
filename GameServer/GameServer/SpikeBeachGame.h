#pragma once

#include "SBUtils.h"

#define WAIT_SEC 30


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
	GameStatus Status();
	void Clear();
	bool SetGame(INT32 gameId, Team redTeam, Team blueTeam);
	bool UserIn(SBUser* UserId);
	bool PlayingSync();
	bool WaitUserSync();
private:
	INT32 _gameId;
	std::mutex _gameMutex;
	std::atomic<GameStatus> _gameStatus;
	std::unordered_map<INT64, SBUser*> _redTeam;
	std::unordered_map<INT64, SBUser*> _blueTeam;
	std::chrono::system_clock::time_point _startTime;
	std::chrono::system_clock::time_point _lastSyncTime;
	std::chrono::system_clock::time_point _waitDeadLine;

	Ball _ball;
	INT16 _redScore;
	INT16 _blueScore;

	void NoticeInGame(std::vector<char>& notify);
	std::vector<char> GenGameSyncData();
};

