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
	
	// �켱 ����� Ŭ��κ���, ���� ��û�� �޴´�
	// Content�� �ϴ� ��⿭�� �ִ´� -> queue?
	// TaskManager�� ��⿭���� �ϳ��� ������ ó���Ѵ�
	// �̶�, ���� ������ �ִ� ������ �ش� ������ sessionId �� userId�� ��ȿ�ϴ�.
	// �׷��Ƿ� �������� ���� �����ϰ�, ���ӿ��� ������ ���·� �ǳ��ִ°� �������ϴ�.
	//std::queue<SBUser> _authCheckWaitQueue;
	//std::mutex _authCheckWaitQueueMutex;

	//std::map<INT64, SBUser> _sessionId2UserMap;
	//std::mutex _sessionId2UserMapMutex;

	void EnterGame(Package package);

	// InfoString parsing functions
};
