#pragma once

#include <ws2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <array>
#include <vector>
#include <optional>
#include <string>
#include <List>
#include <map>
#include <queue>
#include <stack>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <time.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include <math.h>
#include <format>

//https://kyungpyo-kim.github.io/study/thread-safety-of-unordered_map/
#include <shared_mutex>

#include <processthreadsapi.h>

#include <hiredis.h>
#include "./jsoncpp/json/json.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>


#include "Singleton.h"
#include "PacketId.h"
#include "Packet.h"
#include "ErrorCode.h"
#include "Logger.h"
#include "Package.h"
#include "Packet.h"
//#include "SBManager.h"
//#include "SBUserManager.h"
//#include "SessionManager.h"
//#include "TaskManager.h"
//#include "ServerConfigManager.h"
//#include "ThreeValues.h"
//#include "Acceleration.h"
//#include "Position.h"
//#include "Velocity.h"
//#include "SBUtils.h"
//typedef float coordType;
//#define UTC_NOW std::chrono::system_clock::now()
//using SysClock = std::chrono::system_clock;
//using SysTp = std::chrono::system_clock::time_point;
//using UserMotionData = std::tuple<Position, Velocity, Acceleration>;
//typedef UserMotionData std::tuple<Position, Velocity, Acceleration>;
//#define UTC_NOW std::chrono::system_clock::now()

#define TEST_SESSION_ID -10