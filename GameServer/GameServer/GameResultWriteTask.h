#pragma once
#include "Task.h"

class GameResultWriteTask : public Task
{
public:
	GameResultWriteTask();
	~GameResultWriteTask();
	void Do();
private:
	const std::string MATCH_RESULT_QUERY = "INSERT INTO matchs(start_time, finish_time) VALUES(\"{}\", \"{}\");";
	std::string LAST_INSERT_ID = "SELECT LAST_INSERT_ID();";
	std::string RESULT_PER_USER_QUERY = "INSERT INTO match_results_per_user(user_id, match_id, result) VALUES({}, {}, {})";

	sql::Driver* _driver;
	sql::Connection* _con;
	sql::ConnectOptionsMap _connection_properties;
	sql::Statement* _stmt;

	sql::PreparedStatement* _pstmt_match;
	sql::PreparedStatement* _pstmt_getLast;
	sql::PreparedStatement* _pstmt_per_user;

	std::string ConvertTimeStr(std::chrono::system_clock::time_point time);
};

