#include "Client.h"
#include <vector>

//argv[0] is program name, other are arguments we put in.
int main(int argc, char* argv[]) //Makes it possible to pass commands to commandline.
{
	ONet::Client client(0, "Arre");
	client.Init();
	client.Tick();
}