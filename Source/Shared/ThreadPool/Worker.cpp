#include "Worker.h"
#include "Work.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <iostream>
#include <assert.h>
#include <processthreadsapi.h>


Threading::Worker::Worker()
{
}

void Threading::Worker::Init()
{
	myThread = new std::thread(&Worker::WorkThread, this);
}

bool Threading::Worker::IsWorking() const
{
	return myIsWorking;
}

void Threading::Worker::AssignWork(Work* aFunction)
{
	assert(myIsWorking == false && "Already has job assigned");
	myWorkToBeDone = aFunction;
	myIsWorking = true;
}

void Threading::Worker::WorkThread()
{
	while(myHasQuit == false)
	{
		if(myIsWorking == true)
		{
			assert(myWorkToBeDone != nullptr && "My assigned work is nullptr :(");
			 myWorkToBeDone->DoWork();
			 myWorkToBeDone = nullptr;
			 myIsWorking = false;
		}
		else
		{
			std::this_thread::yield();
		}
	}
}
