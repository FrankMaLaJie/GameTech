#include "NetworkBase.h"

NetworkBase::NetworkBase()
{
	netHandle = nullptr;
}

NetworkBase::~NetworkBase()
{
	if (netHandle) {
		enet_host_destroy(netHandle);
	}
}

void NetworkBase::Initialise() {
	enet_initialize();
}

void NetworkBase::Destroy() {
	enet_deinitialize();
}

bool NetworkBase::ProcessPacket(GamePacket * packet, int peerID) {
	PacketHandlerIterator firstHandler;
	bool canHandle = GetPackethandlers(packet-> type,
		firstHandler, lastHandler);
	if (canHandle) {
		for (auto i = firstHandler; i != lastHandler; ++i) {
		}
		return true;
	}
	//std::cout << __FUNCTION__ << "no handler for packet type"
	//	<< packet-> type << std::endl;
	return false;

}