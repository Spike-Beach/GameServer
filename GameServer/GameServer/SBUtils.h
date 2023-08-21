#pragma once
#include "GameObj.h"

struct UserInfo
{
	INT64 id = -1;
	std::string nickName;
};
using Team = std::array<UserInfo, 2>;

struct GameResult
{
	Team winner;
	Team loser;
	std::chrono::system_clock::time_point startTime;
	std::chrono::system_clock::time_point finishTime;
};

enum class GameStatus : UINT16
{
	EMPTY = 0,
	WAITING = 1,
	PLAYING = 2,
	FINISHING = 3,
	SOMEONE_LEANVE = 4,
};


template <typename T>
class ThreadJobQueue
{
public:
	void Push(T data)
	{
		std::unique_lock<std::mutex> lock(_inputMutex);
		_inputQueue->push(data);
	}

	std::optional<T> Pop()
	{
		if (_outputQueue->empty())
		{
			std::unique_lock<std::mutex> lock(_inputMutex);
			std::swap(_inputQueue, _outputQueue);
		}
		if (_outputQueue->empty())
		{
			return nullptr;
		}
		T output = _outputQueue->front();
		_outputQueue->pop();
		return output;
	}

private:
	std::mutex _inputMutex;
	std::queue<T> _queueA;
	std::queue<T> _queueB;
	std::queue<T>* _inputQueue;
	std::queue<T>* _outputQueue;
};