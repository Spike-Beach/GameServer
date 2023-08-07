#pragma once
//#include "SpikeBeachHandler.h"
#include "SBManager.h"

class GameSetTask : public Task
{
public:
	GameSetTask() 
	{
		_conn = redisConnect("127.0.0.1", 6380);
		if (_conn == NULL || _conn->err)
		{
			// log
			//return false;
			throw std::runtime_error("GameSetTask redisConnect error");
		}
		redisReply* reply = (redisReply*)redisCommand(_conn, "AUTH %s", "1q2w3e4r");
		if (reply->type == REDIS_REPLY_ERROR)
		{
			// log
			//return false;
			throw std::runtime_error("GameSetTask redis AUTH error");
		}
		freeReplyObject(reply);
		//return true;
	};

	//bool Init()
	//{
	//	_conn = redisConnect("127.0.0.1", 6380);
	//	if (_conn == NULL || _conn->err)
	//	{
	//		// log
	//		return false;
	//	}
	//	redisReply* reply = (redisReply*)redisCommand(_conn, "AUTH %s", "1q2w3e4r");
	//	if (reply->type == REDIS_REPLY_ERROR)
	//	{
	//		// log
	//		return false;
	//	}
	//	return true;
	//}

	void Do()
	{
		redisReply* reply = (redisReply*)redisCommand(_conn, "SUBSCRIBE %s", "gameInfo");
		//int rt = redisGetReply(_conn, (void**)&reply);
		if (reply != NULL && reply->type == REDIS_REPLY_ARRAY)
		{
			if (reply->elements < 3 
				|| reply->element[2]->type != REDIS_REPLY_STRING
				|| reply->element[2]->str == NULL)
			{
				// log
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
	//std::shared_ptr<SpikeBeachContent> _spikeBeachContent;
	redisContext* _conn;
	INT32 _gameId;
	Team _redTeam;
	Team _blueTeam;
};