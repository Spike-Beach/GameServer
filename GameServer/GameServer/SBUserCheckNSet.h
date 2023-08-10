#pragma once
#include "Task.h"
#include "SpikeBeachHandler.h"
#include "SBUserManager.h"
#include "jsoncpp/json/json.h"
#include "SBManager.h"
#include "GameEnter.h"

class UserCheckNSet : public Task
{
public:
	UserCheckNSet() 
	{
		_conn = redisConnect("127.0.0.1", 6379);
		if (_conn == NULL || _conn->err)
		{
			throw std::runtime_error("UserCheckNSet redisConnect error");
		}
	};

	void Do()
	{
		GameEnterRes res;
		Json::CharReaderBuilder builder;
		Json::Value root;
		JSONCPP_STRING errs;
		SBUser* user = g_SBUserManager.PopAuthWaitingUser();
		if (user == nullptr)
		{
			return;
		}

		std::string cmd = std::string("GET Session:") + user->GetAssignedId();
		redisReply* reply = (redisReply*)redisCommand(_conn, cmd.c_str());
		if (reply != nullptr && reply->type == REDIS_REPLY_STRING)
		{
			std::string str = reply->str;
			std::istringstream jsonStream(str);
			if (Json::parseFromStream(builder, jsonStream, &root, &errs) == false)
			{
				g_logger.Log(LogLevel::ERR, "UserCheckNSet::Do", "Json Error: " + errs);
				res.errorCode = ErrorCode::SessionError;
				g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
				return;
			}
			
			if (root["userAssignedId"].empty() || root["token"].empty() || root["userId"].empty())
			{
				g_logger.Log(LogLevel::CRITICAL, "UserCheckNSet::Do", "Invalid Session Info: " + str);
				res.errorCode = ErrorCode::SessionError;
				g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
				return;
			}
			std::string token = root["token"].asString();
			INT64 userId = root["userId"].asInt64();
			std::string nickname = root["nickname"].asString();

			if (user->GetToken() == token)
			{
				user->SetUserId(userId);
				user->SetUserNickname(nickname);
				if (g_SBManager.UserEnterGame(user->GetGameId(), user) == true)
				{
					res.errorCode = ErrorCode::GameEnterSuccess;
				}
				else // 이미 로그 남겼음
				{
					res.errorCode = ErrorCode::InvalidGame;
				}
			}
			else
			{
				g_logger.Log(LogLevel::ERR, "UserCheckNSet", "Invalid Token : " + std::to_string(userId));
				res.errorCode = ErrorCode::InvalidToken;
			}
			g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
		}
	}
private:
	redisContext* _conn;
};