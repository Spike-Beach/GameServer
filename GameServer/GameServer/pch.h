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

//https://kyungpyo-kim.github.io/study/thread-safety-of-unordered_map/
#include <shared_mutex>

#include <processthreadsapi.h>

#include <hiredis.h>


#include "Singleton.h"
#include "PacketId.h"
#include "Packet.h"
#include "ErrorCode.h"
#include "Logger.h"
#include "Package.h"
#include "Packet.h"
#include "SBManager.h"
#include "SBUserManager.h"
#include "SessionManager.h"
#include "TaskManager.h"


