#include "pch.h"
#include "SpikeBeachHandler.h"
#include "SBUserManager.h"
#include "SBManager.h"
#include "GameEnter.h"
#include "Sync.h"
#include "Controll.h"

SpikeBeachHandler::SpikeBeachHandler()
	: _isRunning(true)
{
	_contentFuncMap.insert(std::make_pair(PacketId::GAME_ENTER_REQ, std::bind(& SpikeBeachHandler::EnterGame, this, std::placeholders::_1)));
	_contentFuncMap.insert(std::make_pair(PacketId::SYNC_REQ, std::bind(&SpikeBeachHandler::SyncGame, this, std::placeholders::_1)));
	_contentFuncMap.insert(std::make_pair(PacketId::CONTROLL_REQ, std::bind(&SpikeBeachHandler::ControllGame, this, std::placeholders::_1)));
}


void SpikeBeachHandler::EnterGame(Package package)
{
	GameEnterReq req;
	GameEnterRes res;
	try
	{
		req.Deserialize(&package._buffer[0], package._buffer.size());
		if (req.packetId == PacketId::ERROR_OCCUR)
		{
			g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::EnterGame", "PacketId::ERROR_OCCUR");
			res.errorCode = ErrorCode::InvalidPacketForm;
			g_sessionManager.SendData(package.sessionId, res.Serialize());
			g_sessionManager.ReleaseSession(package.sessionId, true);
			return;
		}
	}
	catch (std::exception e)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::EnterGame", "GameEnterReq::Deserialize Exception, " + std::string(e.what()));
		res.errorCode = ErrorCode::InvalidPacketForm;
		g_sessionManager.SendData(package.sessionId, res.Serialize());
		g_sessionManager.ReleaseSession(package.sessionId, true);
		return;
	}

	// 유저 매니저에서 빈 유저를 받아옴
	SBUser* user = g_SBUserManager.GetEmptyUser(package.sessionId);
	if (user == nullptr)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::EnterGame", "Not enough empty user");
		res.errorCode = ErrorCode::NotEnoughEmptyUser;
		g_sessionManager.SendData(package.sessionId, res.Serialize());
		return;
	}
	
	// 기본적인 정보 할당 후,  유저 체크앤 셋에 유저를 예약
	user->SetUser(req.userAssignedId, req.token, req.gameId);
	g_SBUserManager.PushAuthWaitingUser(user);
	return ;
}

void SpikeBeachHandler::SyncGame(Package package)
{
	SBUser* user = g_SBUserManager.GetUserBySessionId(package.sessionId);
	if (user == nullptr)
	{
		return;
	}
	SyncReq req;
	try
	{
		req.Deserialize(&package._buffer[0], package._buffer.size());
		if (req.packetId == PacketId::ERROR_OCCUR)
		{
			g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::EnterGame", "PacketId::ERROR_OCCUR");
			g_sessionManager.ReleaseSession(package.sessionId, true);
			return;
		}
	}
	catch (std::exception e)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::EnterGame", "GameEnterReq::Deserialize Exception, " + std::string(e.what()));
		g_sessionManager.ReleaseSession(package.sessionId, true);
		return;
	}

	SpikeBeachGame* game = g_SBManager.GetGame(user->GetGameId());
	if (game == nullptr)
	{
		return;
	}
	game->SetUserTimes(user->GetId(), req.syncReqTime, req.tts);
	g_sessionManager.SendData(package.sessionId, game->GetSerializedSyncPacket(user->GetId()));
}

void SpikeBeachHandler::ControllGame(Package package)
{
	ControllReq req;
	SBUser* user = g_SBUserManager.GetUserBySessionId(package.sessionId);
	if (user == nullptr)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::ControllGame", "User is not exist");
		return;
	}
	SpikeBeachGame* game = g_SBManager.GetGame(user->GetGameId());
	if (game == nullptr)
	{
		g_logger.Log(LogLevel::ERR, "SpikeBeachHandler::ControllGame", "Game is not exist");
		return;
	}
	req.Deserialize(&package._buffer[0], package._buffer.size());
	game->Controll(user->GetId(), req.xCtl, req.yCtl);
}

bool SpikeBeachHandler::IsRuning()
{
	return _isRunning.load();
}