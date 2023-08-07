#pragma once
#include "GameHandler.h"
#include "SpikeBeachGame.h"
#include "TaskManager.h"
#include "SpikeBeachUtils.h"



//template <typename T>
//class ThreadJobQueue
//{
//public:
//	void Push(T data)
//	{
//		std::lock_guard<std::mutex> lock(_inputMutex);
//		_inputQueue->push(data);
//	}
//
//	std::optional<T> Pop()
//	{
//		if (_outputQueue->empty())
//		{
//			std::lock_guard<std::mutex> lock(_inputMutex);
//			std::swap(_inputQueue, _outputQueue);
//		}
//		if (_outputQueue->empty())
//		{
//			return nullptr;
//		}
//		T output = _outputQueue->front();
//		_outputQueue->pop();
//		return output;
//	}
//
//private:
//	std::mutex _inputMutex;
//	std::queue<T> _queueA;
//	std::queue<T> _queueB;
//	std::queue<T>* _inputQueue;
//	std::queue<T>* _outputQueue;
//};

class SpikeBeachHandler : public GameHandler//, public std::enable_shared_from_this<SpikeBeachContent>
{
public:
	SpikeBeachHandler();
	~SpikeBeachHandler() {};
	//bool Init();
	//void Mapping(Package package);

	//void SyncGames();
	//void SetGame(std::string infoStr);
	bool IsRuning();
private:
	std::atomic_bool _isRunning;
	//std::array<SpikeBeachGame, 50> _gamePool;
	//std::map<INT32, SpikeBeachGame* > _gameIdMap;
	//std::queue<SpikeBeachGame* > _emptyGames;
	
	// 우선 연결된 클라로부터, 접속 요청을 받는다
	// Content는 일단 대기열에 넣는다 -> queue?
	// TaskManager는 대기열에서 하나씩 꺼내서 처리한다
	// 이때, 내가 가지도 있는 정보는 해당 유저의 sessionId 및 userId가 유효하다.
	// 그러므로 유저들을 따로 저장하고, 게임에는 포인터 형태로 건내주는게 좋을듯하다.
	//std::queue<SBUser> _authCheckWaitQueue;
	//std::mutex _authCheckWaitQueueMutex;

	//std::map<INT64, SBUser> _sessionId2UserMap;
	//std::mutex _sessionId2UserMapMutex;

	void EnterGame(Package package);

	// InfoString parsing functions
};
