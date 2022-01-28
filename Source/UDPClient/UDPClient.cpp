#include "Client.h"
#include "..\Shared\ThreadPool\ThreadPool.h"

#include <vector>
#include <assert.h>
#include <string>

//argv[0] is program name, other are arguments we put in.
int main(int argc, char* argv[]) //Makes it possible to pass commands to commandline.
{
	auto& threadPool = Threading::ThreadPool::GetInstance();
	threadPool.Init();

	const int port = 54000;
	const std::string address = "127.0.0.1";
	//if (argv[1])
	//{
	//	address = *argv[1];
	//}

	ONet::Client client(port, address, "Arre");
	const bool success = client.Init();

	assert(success && "Client Init failed.");

	while (client.GetIsActive())
	{
		//Thread to update client game logic.
		threadPool.AddASyncWork([&]()
			{
				client.ReceiveData();
			},
			Threading::eWorkType::ReceiveServerData);

		//Thread to handle client connections.
		threadPool.AddASyncWork([&]()
			{
				client.Tick();
			},
			Threading::eWorkType::TICK_CLIENT);

		threadPool.DoAllASyncWork();
		threadPool.UpdateAsyncedWork();
	}

	//End of game.
	while (threadPool.IsDoingAnyASyncWork())
	{
		threadPool.DoAllASyncWork();
		std::this_thread::yield();
		client.ShutDown();
	}
}