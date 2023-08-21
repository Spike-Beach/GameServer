#include "pch.h"
#include "GameResultWriteTask.h"
#include "ServerConfigManager.h"
#include "SBManager.h"
#include "SBUtils.h"

GameResultWriteTask::GameResultWriteTask()
{
	auto gameDataDbConfig = g_config.config["ConnectionStrings"]["GameDataDb"];
	_connection_properties["hostName"] = sql::SQLString(gameDataDbConfig["hostName"].asString());
	_connection_properties["userName"] = sql::SQLString(gameDataDbConfig["userName"].asString());
	_connection_properties["password"] = sql::SQLString(gameDataDbConfig["password"].asString());
	_driver = get_driver_instance();
	_con = _driver->connect(_connection_properties);
	_con->setSchema("sb_gamedata_db");
	_pstmt_match = _con->prepareStatement("INSERT INTO matchs(start_time, finish_time) VALUES(?, ?);");
	_pstmt_getLast = _con->prepareStatement("SELECT LAST_INSERT_ID();");
	_pstmt_per_user = _con->prepareStatement("INSERT INTO match_results_per_user(user_id, match_id, result) VALUES(?, ?, ?)");
}

GameResultWriteTask::~GameResultWriteTask()
{
	delete _pstmt_match;
	delete _pstmt_getLast;
	delete _pstmt_per_user;
	delete _con;
}


void GameResultWriteTask::Do()
{
	std::optional<GameResult> gameResult = g_SBManager.GetGameResult();
	if (gameResult.has_value() == false)
	{
		return;
	}
	try
	{
		_pstmt_match->setString(1, ConvertTimeStr(gameResult->startTime));
		_pstmt_match->setString(2, ConvertTimeStr(gameResult->finishTime));
		_pstmt_match->executeUpdate();
		sql::ResultSet* res = _pstmt_getLast->executeQuery();
		if (res == NULL || res->next() == false)
		{
			g_logger.Log(LogLevel::ERR, "GameResultWriteTask::Do()", "SELECT LAST_INSERT_ID(); fail");
			return;
		}
		INT64 matchId = res->getInt(1);
		const std::vector<std::tuple<int, bool> > results = {
		{ gameResult->winner[0].id, true }, { gameResult->winner[1].id, true },
		{ gameResult->loser[0].id, false }, { gameResult->loser[1].id, false }};
		for (const auto& result : results)
		{
			_pstmt_per_user->setInt(1, std::get<0>(result));
			_pstmt_per_user->setInt(2, matchId);
			_pstmt_per_user->setBoolean(3, std::get<1>(result));
			_pstmt_per_user->executeUpdate();
		}
	}
	catch (sql::SQLException& e)
	{
		g_logger.Log(LogLevel::ERR, "GameResultWriteTask exception", e.what());
		return;
	}
}

std::string GameResultWriteTask::ConvertTimeStr(std::chrono::system_clock::time_point time)
{
	std::time_t currentTime = std::chrono::system_clock::to_time_t(time);
	struct tm localTime;
	std::ostringstream oss;
	localtime_s(&localTime, &currentTime);
	oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");

	return oss.str();
}