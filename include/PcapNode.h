#ifndef PCAP_NODE_H_
#define PCAP_NODE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <deque>
#include "PlatformPcap.h"

class PcapNode
{
public:
	PcapNode(const std::string &ip, const std::string & port, const std::string & ifIp, size_t perSaveTime, size_t totalSaveTime);
	virtual ~PcapNode();

	bool Start();
	void Stop();
	void Update();

private:
	PcapNode() = delete;
	PcapNode(const PcapNode &) = delete;
	PcapNode &operator=(const PcapNode &) = delete;

private:
	std::string _ip;
	std::string _port;
	std::string _ifIp;
	size_t _perSaveTime;
	size_t _totalSaveTime;
	size_t _totalSaveCnt;

	size_t _index;
	std::chrono::time_point<std::chrono::steady_clock> _lastTime;
	std::deque<std::string> _deqFileName;
	PlatformPcapHandle _pcapHdl;
};

#endif // !PCAP_NODE_H_