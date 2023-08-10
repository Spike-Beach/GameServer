#include "pch.h"
#include "ParsedRoomInfo.h"

bool  ParsedRoomInfo::Parse(const std::string& infoStr)
{
	INT16 redTeamCnt = 0;
	INT16 blueTeamCnt = 0;
	std::vector<std::string> allUserNicks;
	std::vector<INT64> allUserIds;
	std::vector<std::string> redTeamNicks;
	std::vector<std::string> blueTeamNicks;

	allUserNicks.reserve(4);
	allUserIds.reserve(4);
	redTeamNicks.reserve(2);
	blueTeamNicks.reserve(2);

	size_t startPos = 0;
	size_t endPos = infoStr.find('\t', startPos);

	while (endPos != std::string::npos) {
		startPos = endPos + 1;
		char dataType = infoStr[startPos];
		startPos += 2;
		endPos = infoStr.find('\t', startPos);

		switch (dataType) 
		{
			case 'I':
			{
				roomId = std::stoi(infoStr.substr(startPos, endPos - startPos));
				break;
			}
			case 'T':
			{
				roomTitle = infoStr.substr(startPos, endPos - startPos);
				break;
			}
			case 'U':
			{
				allUserNicks = SplitData(infoStr.substr(startPos, endPos - startPos));
				break;
			}
			/*case 'H':
				hostUser = splitData(infoStr.substr(startPos, endPos - startPos));
				break;*/
			case 'A':
			{
				redTeamNicks = SplitData(infoStr.substr(startPos, endPos - startPos));
				break;
			}
			case 'B':
			{
				blueTeamNicks = SplitData(infoStr.substr(startPos, endPos - startPos));
				break;
			}
			case 'u':
			{
				auto userIdsStr = SplitData(infoStr.substr(startPos, endPos - startPos));
				for (size_t i = 0; i < userIdsStr.size(); i++)
				{
					allUserIds.push_back(std::stoll(userIdsStr[i]));
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	//redTeam[0] = UserInfo{ findUserId(allUserNicks, allUserIds, redTeamNicks[0]), redTeamNicks[0] };
	//redTeam[1] = UserInfo{ findUserId(allUserNicks, allUserIds, redTeamNicks[1]), redTeamNicks[1] };
	for (size_t i = 0; i < redTeamNicks.size(); i++)
	{
		redTeam[i] = UserInfo{ findUserId(allUserNicks, allUserIds, redTeamNicks[i]), redTeamNicks[i] };
	}
	/*blueTeam[0] = UserInfo{ findUserId(allUserNicks, allUserIds, blueTeamNicks[0]), blueTeamNicks[0] };
	blueTeam[1] = UserInfo{ findUserId(allUserNicks, allUserIds, blueTeamNicks[1]), blueTeamNicks[1] };*/
	for (size_t i = 0; i < blueTeamNicks.size(); i++)
	{
		blueTeam[i] = UserInfo{ findUserId(allUserNicks, allUserIds, blueTeamNicks[i]), blueTeamNicks[i] };
	}
	
	return CheckParsedInfo();
}

std::vector<std::string> ParsedRoomInfo::SplitData(const std::string& infoStr)
{
	std::vector<std::string> result;
	size_t startPos = 0;
	size_t endPos = 0;

	while (endPos != std::string::npos)
	{
		startPos = startPos = infoStr.find_first_not_of(' ', endPos);
		if (startPos == std::string::npos)
		{
			break;
		}
		endPos = infoStr.find(' ', startPos + 1);
		result.push_back(infoStr.substr(startPos, endPos - startPos));
	}

	return result;
}

bool ParsedRoomInfo::CheckParsedInfo()
{
	if (roomId < 0 || roomTitle.empty()
		|| redTeam[0].id < 0 || redTeam[0].nickName.empty()
		|| redTeam[1].id < 0 || redTeam[1].nickName.empty()
		|| blueTeam[0].id < 0 || blueTeam[0].nickName.empty()
		|| blueTeam[1].id < 0 || blueTeam[1].nickName.empty() )
	{
		return false;
	}

	return true;
}

INT64 ParsedRoomInfo::findUserId(std::vector<std::string>& allNick, std::vector<INT64>& allIds, std::string nick) throw (std::runtime_error)
{
	for (INT64 i = 0; i < allNick.size(); i++)
	{
		if (allNick[i] == nick)
		{
			return allIds[i];
		}
	}
	return -1;
}