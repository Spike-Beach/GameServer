#pragma once
#include "ServerConfigManager.h"
#include "SBManager.h"
#include "SBUtils.h"

class GameSetTask : public Task
{
public:
	GameSetTask() 
	{
		_ip = g_config.config["ConnectionStrings"]["RoomDb"]["Ip"].asString();
		_port = g_config.config["ConnectionStrings"]["RoomDb"]["Port"].asInt();
		_password = g_config.config["ConnectionStrings"]["RoomDb"]["Password"].asString();
		_conn = redisConnect(_ip.c_str(), _port);
		if (_conn == NULL || _conn->err)
		{
			throw std::runtime_error("GameSetTask redisConnect error");
		}
		redisReply* reply = (redisReply*)redisCommand(_conn, "AUTH %s", _password.c_str());
		if (reply->type == REDIS_REPLY_ERROR)
		{
			throw std::runtime_error("GameSetTask redis AUTH error");
		}
		freeReplyObject(reply);
	};


	void Do()
	{
		redisReply* reply = (redisReply*)redisCommand(_conn, "SUBSCRIBE %s", "gameInfo");
		if (reply != NULL && reply->type == REDIS_REPLY_ARRAY)
		{
			if (reply->elements < 3 
				|| reply->element[2]->type != REDIS_REPLY_STRING
				|| reply->element[2]->str == NULL)
			{
				return;
			}

			if (g_SBManager.IsRuning())
			{
				g_SBManager.SetGame(reply->element[2]->str);
			}
			freeReplyObject(reply);
		}
	}

private:
	redisContext* _conn;
	std::string _ip;
	INT32 _port;
	std::string _password;

	INT32 _gameId;
	Team _redTeam;
	Team _blueTeam;
};