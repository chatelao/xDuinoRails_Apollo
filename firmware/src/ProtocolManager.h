#ifndef PROTOCOL_MANAGER_H
#define PROTOCOL_MANAGER_H

#include "cv_definitions.h"
#include "CVManager.h"

enum class Protocol {
    UNDECIDED,
    DCC,
    MM
};

class ProtocolManager {
public:
    ProtocolManager(CVManager& cvManager);
    void begin();
    Protocol getActiveProtocol() const;
    void notifyDccPacket();
    void notifyMmPacket();

private:
    CVManager& _cvManager;
    Protocol _activeProtocol;
    bool _dccEnabled;
    bool _mmEnabled;
};

#endif // PROTOCOL_MANAGER_H
