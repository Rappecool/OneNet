#include "ThreadPool.h"
#include "Work.h"
#include "Worker.h"
#include <assert.h>

Threading::ThreadPool::ThreadPool()
{
	for (size_t i = 0; i < myWorkers.size(); i++)
	{
		myWorkers.at(i) = nullptr;
	}
}

void Threading::ThreadPool::Init()
{
	for (size_t i = 0; i < myWorkers.size(); i++)
	{
		assert(myWorkers.at(i) == nullptr && "Worker not nullptr only init once!");
		if (myWorkers.at(i) != nullptr)
		{
			continue;
		}
		myWorkers.at(i) = new Worker();
		myWorkers.at(i)->Init();
	}
}

void Threading::ThreadPool::Unload()
{
	//TODO: FIX Unloading threads when quitting.

	//for (size_t i = 0; i < myWorkers.size(); i++)
	//{
	//	assert(myWorkers.at(i) == nullptr && "Worker not nullptr only init once!");
	//	if (myWorkers.at(i) != nullptr)
	//	{
	//		continue;
	//	}
	//	myWorkers.at(i) = new Worker();
	//	myWorkers.at(i)->Init();
	//}
}

Threading::ThreadPool& Threading::ThreadPool::GetInstance()
{
	//assert(myInstance != nullptr && "Instance == nullptr, Init the singleton :(");
	static ThreadPool instance;
	return instance;
}


void Threading::ThreadPool::DoAllWork()
{
	assert(myIsDoingAllWork == false && "Is already doing all work");
	if (myIsDoingAllWork)
	{
		return;
	}
	myIsDoingAllWork = true;
	WorkOnList(myWork);
	myIsDoingAllWork = false;
}



void Threading::ThreadPool::WorkOnList(std::vector<Work*>& someWork)
{
	std::vector<Work*> assignedWork;

	int workLeft = static_cast<int>(someWork.size()); //aparently not in use

	while (someWork.size() > 0)
	{
		for (size_t i = 0; i < myWorkers.size(); i++)
		{
			if (someWork.size() <= 0)
			{
				break;
			}

			myMutex.lock();
			if(myWorkers.at(i)->IsWorking() == false)
			{
				assignedWork.push_back(someWork.at(0));
				myWorkers.at(i)->AssignWork(someWork.at(0));

				someWork.at(0) = someWork.back();
				someWork.erase(someWork.end() - 1);
			}
			myMutex.unlock();
		}
	}

	WaitUntillWorkIsDone(assignedWork);
}

void Threading::ThreadPool::WaitUntillWorkIsDone(std::vector<Threading::Work*>& someWork)
{
	for (size_t i = 0; i < someWork.size(); i++)
	{
		if (someWork.at(i)->IsDone() == false)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			--i;
		}
	}

	for (size_t i = 0; i < someWork.size(); i++)
	{
		someWork.erase(someWork.begin());
	}
}

void Threading::ThreadPool::DoAllASyncWork()
{
	WorkOnUnsyncedList(myASyncWork);
}

void Threading::ThreadPool::WorkOnUnsyncedList(std::array<Work*, static_cast<unsigned int>(eWorkType::COUNT)>& someWork)
{
	/*std::vector<Work*> assignedWork;
	std::vector<Work*> tempWork;*/

	//int workLeft = static_cast<int>(someWork.size()); // no initilized references
	bool hasAssignedAllWork = false;

	while (hasAssignedAllWork == false)
	{
		hasAssignedAllWork = true;

		for (size_t i = 0; i < someWork.size(); i++)
		{
			if (!someWork[i])
				continue;
			
			for (size_t j = 0; j < myWorkers.size(); j++)
			{
				bool breakMe = false;
				myMutex.lock();
				if (someWork.at(i) != nullptr && myWorkers.at(j)->IsWorking() == false)
				{
					myActiveASyncWork.at(i) = someWork.at(i);
					myWorkers.at(j)->AssignWork(myActiveASyncWork.at(i));
					someWork.at(i) = nullptr;
					breakMe = true;
				}
				myMutex.unlock();
				if(breakMe)
					break;
			}
		}

		for (size_t i = 0; i < someWork.size(); i++)
		{
			if (someWork.at(i) != nullptr) 
			{
				hasAssignedAllWork = false;
			}
		}
	}

	/*while (tempWork.size() > 0)
	{
		for (size_t i = 0; i < myWorkers.size(); i++)
		{
			if (tempWork.size() <= 0)
			{
				break;
			}

			myMutex.lock();
			if (myWorkers.at(i)->IsWorking() == false)
			{
				assignedWork.push_back(tempWork.at(0));
				myWorkers.at(i)->AssignWork(tempWork.at(0));

				tempWork.at(0) = tempWork.back();
				tempWork.erase(tempWork.end() - 1);
			}
			myMutex.unlock();
		}
	}

	myActiveASyncWork = assignedWork;*/
	//CheckAsyncedWork();
}

void Threading::ThreadPool::UpdateAsyncedWork()
{
	//PIXScopedEvent(PIX_COLOR(200, 100, 50), "Update ASync Work");

	bool isWorking = false;
	for (size_t i = 0; i < myActiveASyncWork.size(); i++)
	{
		if (!IsDoingASyncWork(i))
		{
			if (auto* work = myActiveASyncWork.at(i))
			{
				delete myActiveASyncWork.at(i);
				myActiveASyncWork.at(i) = nullptr;
			}

			myActiveWorkTypes.at(i) = false;
			myASyncWork.at(i) = nullptr;
		}
	}

	//myIsDoingAllAsyncWork = isWorking;
	//if (isWorking == false)
	//{
	//	for (size_t i = 0; i < myActiveASyncWork.size(); i++)
	//	{
	//		myActiveASyncWork.erase(myActiveASyncWork.begin());
	//	}
	//	//return myIsDoingAllAsyncWork;
	//}
}

