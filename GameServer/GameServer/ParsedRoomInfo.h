#pragma once
#include "SBUtils.h"

class ParsedRoomInfo
{
public:
	bool Parse(const std::string& infoStr);
	std::vector<std::string> SplitData(const std::string& infoStr);
	INT64 findUserId(std::vector<std::string>& allNick, std::vector<INT64>& allIds, std::string nick);
	INT64 roomId = -1;
	std::string roomTitle;
	Team redTeam;
	Team blueTeam;

private:
	bool CheckParsedInfo();
};

