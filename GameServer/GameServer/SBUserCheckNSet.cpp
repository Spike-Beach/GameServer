#include "pch.h"
#include "SBUserCheckNSet.h"
#include "SBUserManager.h"
#include "SBManager.h"
#include "ServerConfigManager.h"
#include "SessionManager.h"
#include "GameEnter.h"
#include "SBUser.h"

SBUserCheckNSet::SBUserCheckNSet()
{
	_ip = g_config.config["ConnectionStrings"]["SessionDb"]["Ip"].asString();
	_port = g_config.config["ConnectionStrings"]["SessionDb"]["Port"].asInt();
	_conn = redisConnect(_ip.c_str(), _port);
	if (_conn == NULL || _conn->err)
	{
		throw std::runtime_error("UserCheckNSet redisConnect error");
	}
};

void SBUserCheckNSet::Do()
{
	// FOT TEST
	static INT64 XXX_TEST_ID_XXX = 0;

	GameEnterRes res;
	res.errorCode = ErrorCode::SessionError;
	Json::CharReaderBuilder builder;
	Json::Value config;
	JSONCPP_STRING errs;
	SBUser* user = g_SBUserManager.PopAuthWaitingUser();
	if (user == nullptr)
	{
		return;
	}

	//std::string cmd = std::string("GET Session:") + user->GetAssignedId();
	//redisReply* reply = (redisReply*)redisCommand(_conn, cmd.c_str());
	//if (reply != nullptr && reply->type == REDIS_REPLY_STRING)
	//{
	//	std::string str = reply->str;
	//	std::istringstream jsonStream(str);
	//	if (Json::parseFromStream(builder, jsonStream, &config, &errs) == false)
	//	{
	//		g_logger.Log(LogLevel::ERR, "UserCheckNSet::Do", "Json Error: " + errs);
	//		res.errorCode = ErrorCode::SessionError;
	//		g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
	//		return;
	//	}

	//	if (config["userAssignedId"].empty() || config["token"].empty() || config["userId"].empty())
	//	{
	//		g_logger.Log(LogLevel::CRITICAL, "UserCheckNSet::Do", "Invalid Session Info: " + str);
	//		res.errorCode = ErrorCode::SessionError;
	//		g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
	//		return;
	//	}
	//	std::string token = config["token"].asString();
	//	INT64 userId = config["userId"].asInt64();
	//	std::string nickname = config["nickname"].asString();

		//if (user->GetToken() == token)
		//{
		//user->SetUserId(userId);
		//user->SetUserNickname(nickname);
		//if (g_SBManager.UserEnterGame(user->GetGameId(), user) == true)
		//{
		//	res.errorCode = ErrorCode::GameEnterSuccess;
		//}
		//else // 이미 로그 남겼음
		//{
		//	res.errorCode = ErrorCode::InvalidGame;
		//}
		/*}
		else
		{
			g_logger.Log(LogLevel::ERR, "UserCheckNSet", "Invalid Token : " + std::to_string(userId));
			res.errorCode = ErrorCode::InvalidToken;
		}*/
		{ // FOR TEST
			std::array<SBUser*, 4> userArray;
			userArray[0] = user;
			userArray[1] = g_SBUserManager.GetEmptyUser(TEST_SESSION_ID);
			userArray[2] = g_SBUserManager.GetEmptyUser(TEST_SESSION_ID - 1);
			userArray[3] = g_SBUserManager.GetEmptyUser(TEST_SESSION_ID - 2);
			for (int i = 0; i < 4; i++)
			{
				userArray[i]->SetUserId(XXX_TEST_ID_XXX);
				userArray[i]->SetUserNickname(std::string("TEST_NICK") + std::to_string(XXX_TEST_ID_XXX));
				userArray[i]->SetUser(std::string("TEST_ASSIGNED_ID") + std::to_string(XXX_TEST_ID_XXX), "token", user->GetGameId());
				++XXX_TEST_ID_XXX;
			}

			//gen string:"\tI 4326\tT TestRoom\tU gyeon2 gyeon4 gyeon gyeon3\tA gyeon4 gyeon2\tB gyeon3 gyeon\tu 2 4 1 3"
			std::string testRoomInfoStr = std::string("\tI ") + std::to_string(user->GetGameId()) + "\tT TestRoom" + std::to_string(user->GetGameId()) + "\tU ";
			for (int i = 0; i < 4; i++)
			{
				testRoomInfoStr += userArray[i]->GetNickName() + " ";
			}
			testRoomInfoStr += "\tA " + userArray[0]->GetNickName() + " " + userArray[1]->GetNickName() + "\tB " + userArray[2]->GetNickName() + " " + userArray[3]->GetNickName() + "\tu ";
			for (int i = 0; i < 4; i++)
			{
				testRoomInfoStr += std::to_string(userArray[i]->GetId()) + " ";
			}
			g_SBManager.SetGame(testRoomInfoStr);

			for (int i = 0; i < 4; i++)
			{
				if (g_SBManager.UserEnterGame(user->GetGameId(), userArray[i]) == true)
				{
					res.errorCode = ErrorCode::GameEnterSuccess;
				}
			}
			g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
			return;
		}
		//g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
		//return;
	//}
	//g_logger.Log(LogLevel::ERR, "UserCheckNSet", "Invalid userId: " + user->GetAssignedId());
	//g_sessionManager.SendData(user->GetSessionId(), res.Serialize());
}
