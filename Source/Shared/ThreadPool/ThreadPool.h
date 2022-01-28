#pragma once
#include <functional>
#include <thread>
#include <array>
#include <vector>
#include "TemplatedWork.h"
#include <iostream>
#include <mutex>

#include <map>

////// EXAMPLE CODE FOR ASYNC
//// check if you are already are doing AllAsyncWork
//if (Threading::ThreadPool::GetInstance().IsDoingAllASyncWork() == false)
//{
//	//if not add your async work to threadpool
//	Threading::ThreadPool::GetInstance().AddASyncWork([&]() {
//
//		LoadModels(chestModelInstance, pedalModelInstacne, buddahModelInstance, scene, camera,
//			gameFinishedLoading, gameLogicDone, gameFinishedLoading);
//
//		});
//
//	//Start doing all AsyncWork
//	Threading::ThreadPool::GetInstance().DoAllASyncWork();
//}
//else
//{
//	//If you already are doing your asyncwork, update it to know when it is done
//	Threading::ThreadPool::GetInstance().UpdateAsyncedWork();
//}

namespace Threading
{
	class Worker;
	class Work;

	enum class eWorkType
	{
		ReceiveClientData,
		ReceiveServerData,
		Tick_SERVER,
		TICK_CLIENT,
		Send,
		COUNT
	};

	class ThreadPool
	{
	public:

	public:
		ThreadPool();
		~ThreadPool() = default;

		void Init();

		void Unload();

		static ThreadPool& GetInstance();

		inline std::mutex& GetMutex() { return myMutex; }

		template<class T>
		void AddWork(const T& aFunction);

		template<class T>
		void AddASyncWork(const T& aFunction, eWorkType aWorkType);

		void DoAllWork();
		void DoAllASyncWork();

		void UpdateAsyncedWork();

		const bool IsDoingASyncWork(const unsigned int aWork) const
		{
			if (!myActiveWorkTypes[aWork])
				return false;

			if (!myActiveASyncWork[aWork])
				return false;
			else
				return !myActiveASyncWork[aWork]->IsDone();
		}

		const bool IsDoingAnyASyncWork() const
		{
			for (unsigned int i = 0; i < static_cast<unsigned int>(eWorkType::COUNT); i++)
			{
				if (IsDoingASyncWork(i))
					return true;
			}
			return false;
		}

	private:
		void WaitUntillWorkIsDone(std::vector<Work*>& someWork);
		void WorkOnList(std::vector<Work*>& someWork);

		void WorkOnUnsyncedList(std::array<Work*, static_cast<unsigned int>(eWorkType::COUNT)>& someWork);

	private:
		std::array<Worker*, static_cast<unsigned int>(eWorkType::COUNT)> myWorkers = { nullptr };
		std::vector<Work*> myWork;
		std::array<Work*, static_cast<unsigned int>(eWorkType::COUNT)> myASyncWork = { nullptr };
		std::array<Work*, static_cast<unsigned int>(eWorkType::COUNT)> myActiveASyncWork = { nullptr };

		//static ThreadPool myInstance;

		std::mutex myMutex;
		std::atomic<bool> myIsDoingAllWork		= false;

		std::array<bool, static_cast<unsigned int>(eWorkType::COUNT)> myActiveWorkTypes = { false };
	};

	template<class T>
	void ThreadPool::AddWork(const T& aFunction)
	{
		myMutex.lock();
		myWork.push_back(new TemplatedWork<T>(aFunction));
		myMutex.unlock();
	}

	template<class T>
	void ThreadPool::AddASyncWork(const T& aFunction, eWorkType aWorktype)
	{
		myMutex.lock();
		if (!IsDoingASyncWork(static_cast<unsigned int>(aWorktype)))
		{
			myActiveWorkTypes.at(static_cast<unsigned int>(aWorktype)) = true;
			myASyncWork.at(static_cast<unsigned int>(aWorktype)) = (new TemplatedWork<T>(aFunction));
		}
		myMutex.unlock();
	}
}
