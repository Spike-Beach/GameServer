#include "pch.h"
#include "SpikeBeachHandler.h"
//#include "ParsedRoomInfo.h"
#include "GameSetTask.h"
#include "GameSyncTask.h"
#include "GameEnter.h"
#include "SBUserManager.h"
#include "SBManager.h"

SpikeBeachHandler::SpikeBeachHandler()
	: _isRunning(true)
{
	_contentFuncMap.insert(std::make_pair(PacketId::GAME_ENTER_REQ, std::bind(& SpikeBeachHandler::EnterGame, this, std::placeholders::_1)));
	//
	//for (size_t i = 0; i < _gamePool.size(); i++)
	//{
	//	_emptyGames.push(&_gamePool[i]);
	//}
}

//void SpikeBeachHandler::Mapping(Package package)
//{
//	auto func = _contentFuncMap.find(package.packet.GetPacketId());
//	if (func == _contentFuncMap.end())
//	{
//		// log
//		g_sessionManager.ReleaseSession(package.sessionId, true);
//	}
//	func->second(package);
//}

//bool SpikeBeachHandler::Init()
//{
	//auto gameSetTaskPtr = std::make_unique<GameSetTask>(shared_from_this());
	//if (!gameSetTaskPtr->Init())
	//{
	//	return false;
	//}
	//g_TaskManager.AddTask(std::move(gameSetTaskPtr), 100, 0, ONLY_GAME_SET_TASK_PROCSSER_ID);
	//g_TaskManager.AddTask(std::make_unique<GameSyncTask>(shared_from_this()), 100, 0, GAME_TASK_PROCESSER_ID);
//
//	return true;
//}

void SpikeBeachHandler::EnterGame(Package package)
{
	std::cout << "test" << std::endl;
	GameEnterReq req;
	GameEnterRes res;
	try
	{
		req.Deserialize(&package._buffer[0], package._buffer.size());
		if (req.packetId == PacketId::ERROR_OCCUR)
		{
			res.errorCode = ErrorCode::InvalidPacketForm;
			g_sessionManager.SendData(package.sessionId, res.Serialize());
			g_sessionManager.ReleaseSession(package.sessionId, true);
			return;
		}
	}
	catch (std::exception e)
	{
		//log
		res.errorCode = ErrorCode::InvalidPacketForm;
		g_sessionManager.SendData(package.sessionId, res.Serialize());
		g_sessionManager.ReleaseSession(package.sessionId, true);
		return;
	}

	// 유저 매니저에서 빈 유저를 받아옴
	SBUser* user = g_SBUserManager.GetEmptyUser(package.sessionId);
	if (user == nullptr)
	{
		// log, 
		res.errorCode = ErrorCode::NotEnoughEmptyUser;
		g_sessionManager.SendData(package.sessionId, res.Serialize());
		return;
	}
	// 기본적인 정보 할당 후,  유저 체크앤 셋에 유저를 예약
	user->SetUser(req.userAssignedId, req.token, req.gameId);
	//    유저 체크앤 셋에서 검증 후 유저를 게임메니저로 게임에 넣음.
	g_SBUserManager.PushAuthWaitingUser(user);
	return ;
}

//void SpikeBeachContent::SyncGames()
//{
//	for (size_t i = 0; i < _gamePool.size(); i++)
//	{
//		if (_gamePool[i].Status() == GameStatus::PLAYING)
//		{
//			_gamePool[i].Sync();
//		}
//	}
//}

//void SpikeBeachContent::SetGame(std::string infoStr)
//{
//	std::cout << "SetGame" << std::endl;
//	if (_emptyGames.empty() == false)
//	{
//		ParsedRoomInfo info;
//		try
//		{
//			if (info.Parse(infoStr) == false)
//			{
//				std::cout << "invalid info" << std::endl;
//			}
//		}
//		catch (std::exception e)
//		{
//			std::cout << e.what() << std::endl;
//		}
//		SpikeBeachGame* emptyGame = _emptyGames.front();
//		emptyGame->SetGame(info.roomId, info.redTeam, info.blueTeam);
//	}
//}

bool SpikeBeachHandler::IsRuning()
{
	return _isRunning.load();
}