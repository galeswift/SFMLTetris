
namespace Raknet
{
	struct Packet;
	class RakPeerInterface;
}

class TetrisNetworkBase
{
public:
	virtual bool Init() = 0;
	virtual bool IsServer() = 0;
	virtual void Loop() = 0;
	virtual ~TetrisNetworkBase() {};

	// Holds packets
	RakNet::Packet* p;
	RakNet::SystemAddress m_clientID;
	RakNet::RakPeerInterface* m_interface;
};

class TetrisClient : public TetrisNetworkBase
{
public:
	TetrisClient(std::string& ipAddr);
	virtual ~TetrisClient();
	bool Init() override;
	bool IsServer() { return false; }
	void Loop() override;

	std::string m_ipAddress;
};

class TetrisServer : public TetrisNetworkBase
{
public:
	virtual ~TetrisServer();
	bool Init() override;
	void Loop() override;
	bool IsServer() { return true; }
};