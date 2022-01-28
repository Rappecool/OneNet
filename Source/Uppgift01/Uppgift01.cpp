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
		//Thread to handle client connections.
		threadPool.AddASyncWork([&]()
		{
			server.ReceiveClientData();
		},
		Threading::eWorkType::ReceiveClientData);

		//Thread to update server game logic.
		threadPool.AddASyncWork([&]()
		{
				server.Tick();
		},
		Threading::eWorkType::Tick_SERVER);

		////Thread to handle incoming client data.
		//threadPool.AddASyncWork([&]()
		//{
		//		server.ReceiveClientData();
		//},
		//Threading::eWorkType::ReceiveClientData);

		//Update threadpool.
		threadPool.DoAllASyncWork();
		threadPool.UpdateAsyncedWork();
	}

	//End of game.
	while (threadPool.IsDoingAnyASyncWork())
	{
		threadPool.DoAllASyncWork();
		std::this_thread::yield();
		server.ShutDown();
	}

}