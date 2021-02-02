#include "PlatformFile.h"
#include "PcapNode.h"

PcapNode::PcapNode(const std::string &ip, const std::string & port, const std::string & ifIp, size_t perSaveTime, size_t totalSaveTime)
{
	_ip = ip;
	_port = port;
	_ifIp = ifIp;
	_perSaveTime = perSaveTime;
	_totalSaveTime = totalSaveTime;

	if (_perSaveTime > 0)
	{
		_totalSaveCnt = _totalSaveTime / _perSaveTime;
	}
	else
	{
		_totalSaveCnt = 10;
	}

	_index = 0;
	_lastTime = std::chrono::steady_clock::now();
	_pcapHdl = nullptr;
}

PcapNode::~PcapNode()
{
	Stop();
}

bool PcapNode::Start()
{
	std::string sFilter;
	if ("any" != _ip)
	{
		sFilter += "host ";
		sFilter += _ip;

		if ("any" != _port)
		{
			sFilter += " and port ";
			sFilter += _port;
		}
	}
	else
	{
		if ("any" != _port)
		{
			sFilter += "port ";
			sFilter += _port;
		}
	}

	_lastTime = std::chrono::steady_clock::now();
	std::string sFileName = _ip + "_" + _port + "_" + _ifIp + "/";
	PFMakeDirectory(sFileName.c_str());
	sFileName += std::to_string(++_index) + "_" + std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) + ".pcap";
	_deqFileName.emplace_back(sFileName);

	_pcapHdl = PlatformPcapDumpCreate(sFilter.c_str(), _ifIp.c_str(), 0, sFileName.c_str());
	if (!_pcapHdl)
	{
		return false;
	}

	std::cout << "ip:" << _ip << " port:" << _port << " ifIp:" << _ifIp << " start!\n";
	PlatformPcapStart(_pcapHdl);

	return true;
}

void PcapNode::Stop()
{
	PlatformPcapDestroy(&_pcapHdl);
}

void PcapNode::Update()
{
	std::chrono::time_point<std::chrono::steady_clock> nowTime = std::chrono::steady_clock::now();
	if (nowTime - _lastTime > std::chrono::seconds(_perSaveTime))
	{
		_lastTime = nowTime;
		std::string sFileName = _ip + "_" + _port + "_" + _ifIp + "/";
		PFMakeDirectory(sFileName.c_str());
		sFileName += std::to_string(++_index) + "_" + std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) + ".pcap";
		while (_deqFileName.size() >= _totalSaveCnt)
		{
			PFRemoveFile(_deqFileName.front().c_str());
			_deqFileName.pop_front();
		}
		_deqFileName.emplace_back(sFileName);
		PlatformPcapUpdateDumpFileName(_pcapHdl, sFileName.c_str());
	}
}