#pragma once
#include <thread>
#include <functional>
#include <atomic>

namespace Threading
{
	class Work;

	class Worker
	{
	public:
		Worker();

		void Init();

		bool IsWorking() const;
		void AssignWork(Work* aFunction);
		void WorkThread();

	private:
		std::thread* myThread = nullptr;
		std::atomic<bool> myIsWorking = false;
		std::atomic<bool> myHasQuit = false;

		//void* myWorkToBeDone	= nullptr;

		Work* myWorkToBeDone = nullptr;
	};
}