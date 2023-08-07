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

	//void Init()
	//{
	//	_conn = redisConnect("127.0.0.1", 6379);
	//	if (_conn == NULL || _conn->err)
	//	{
	//		// log
	//		//return false;
	//		throw std::runtime_error("UserCheckNSet redisConnect error");
	//	}
	//	//return true;
	//}
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
				std::cerr << "Json Error: " << errs << std::endl;
				res.errorCode = ErrorCode::SessionError;
				g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
				return;
			}
			
			if (root["userAssignedId"].empty() || root["token"].empty() || root["userId"].empty())
			{
				std::cerr << "Json Error: " << "userAssignedId / token invalid" << std::endl;
				res.errorCode = ErrorCode::SessionError;
				g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
				return;
			}
			std::string userAssignedId = root["userAssignedId"].asString();
			std::string token = root["token"].asString();
			INT64 userId = root["userId"].asInt64();

			if (user->GetToken() == token)
			{
				user->SetUserId(userId);
				if (g_SBManager.UserEnterGame(user->GetGameId(), user) == true)
				{
					res.errorCode = ErrorCode::GameEnterSuccess;
				}
				else
				{
					// log
					res.errorCode = ErrorCode::InvalidGame;
				}
			}
			else
			{
				// log
				res.errorCode = ErrorCode::InvalidToken;
			}
			g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
		}
	}
private:
	redisContext* _conn;
};