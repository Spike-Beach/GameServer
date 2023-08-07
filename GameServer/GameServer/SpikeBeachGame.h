#pragma once

#include "SpikeBeachUtils.h"
// 필요한 유저 정보 : 위치, 속도, 가속도, 

#define WAIT_SEC 30

//class SBUser
//{
//public:
//	void Set(UserInfo userInfo)
//	{
//		_userInfo = userInfo;
//	}
//	void Clear()
//	{
//		_userInfo.id = 0;
//		_userInfo.nickName = "";
//		_latency = std::chrono::milliseconds(0);
//	}
//	UserInfo GetUserInfo()
//	{
//		return _userInfo;
//	}
//	//void Clear();
//	//void Sync();
//	//bool renewalInfo(...)
//	//.. getUserInfo
//private:
//	UserInfo _userInfo;
//	std::chrono::milliseconds _latency;
//};

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
	//bool UserOut(INT32 sessionId);
	bool PlayingSync();
	bool WaitUserSync();
private:
	INT32 _gameId;
	std::mutex _gameMutex;
	std::atomic<GameStatus> _gameStatus;
	// 0 : redP1 / 1 : redP2 / 2 : blueP1 / 3 : blueP2
	//std::array<GameUser, 4> _users;
	//std::map<INT32, GameUser*> _session2UserMap;
	std::unordered_map<INT64, SBUser*> _redTeam;
	std::unordered_map<INT64, SBUser*> _blueTeam;
	std::chrono::system_clock::time_point _startTime;
	std::chrono::system_clock::time_point _lastSyncTime;
	std::chrono::system_clock::time_point _waitDeadLine;

	Ball _ball;
	// const MAP _mapInfo;
	INT16 _redScore;
	INT16 _blueScore;

	void SyncNotice();
};

