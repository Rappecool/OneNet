#include "Server.h"
#include "..\Shared\ThreadPool\ThreadPool.h"

int main()
{
	auto& threadPool = Threading::ThreadPool::GetInstance();
	threadPool.Init();
	//startup Winsock.
	ONet::Server server;
	server.Init();

	while (true)
	{
		threadPool.AddASyncWork([&]()
		{
				server.Tick();
		},
		Threading::eWorkType::Listen);

		threadPool.DoAllASyncWork();
		threadPool.UpdateAsyncedWork();
	}

	//End of game.
	while (threadPool.IsDoingAnyASyncWork())
	{
		threadPool.DoAllASyncWork();
		std::this_thread::yield();
	}

}