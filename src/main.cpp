#include <signal.h>
#include <vector>
#include <thread>
#include "PcapNode.h"

static bool bAppStart = true;
static void SigIntHandler(int sig_num)
{
	signal(SIGINT, SigIntHandler);
	bAppStart = false;
}

static void ShowUsage()
{
	std::cerr << "\nUsage:\n"
		<< "create a file named 'input.txt' that contains several lines in <ip port interface perFileSaveTime(s) totalFileSaveTime(s)> format.\n"
		<< "e.g., input.txt\n"
		<< "========================\n"
		<< "224.2.3.4 12345 eno1 60 600\n"
		<< "192.168.0.1 12345 any 60 600\n"
		<< "any 12345 any 120 3600\n"
		<< "========================\n";
}

int main(int argc, char *argv[])
{
	bAppStart = true;
	signal(SIGINT, SigIntHandler);

	if (argc > 1)
	{
		ShowUsage();
	}

	std::ifstream fin("input.txt", std::ios::in);
	if (!fin.is_open())
	{
		std::cerr << "Can not open input.txt!\n";
		ShowUsage();
		return 0;
	}

	std::vector<PcapNode *> nodes;

	std::string ip, port, ifIp;
	size_t perSaveTime = 0;
	size_t totalSaveTime = 0;
	while (fin >> ip >> port >> ifIp >> perSaveTime >> totalSaveTime)
	{
		PcapNode *pNode = new PcapNode(ip, port, ifIp, perSaveTime, totalSaveTime);
		pNode->Start();
		nodes.emplace_back(pNode);
	}

	while (bAppStart)
	{
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			nodes[i]->Update();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		delete nodes[i];
	}
	nodes.clear();

	return 0;
}