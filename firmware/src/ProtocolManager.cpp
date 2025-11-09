#include "ProtocolManager.h"

ProtocolManager::ProtocolManager(CVManager& cvManager) : _cvManager(cvManager), _activeProtocol(Protocol::UNDECIDED) {}

void ProtocolManager::begin() {
    uint8_t cv12 = _cvManager.readCV(CV_POWER_SOURCE_LOCK);
    _dccEnabled = (cv12 & CV12_DCC_ENABLE_BIT) == 0;
    _mmEnabled = (cv12 & CV12_MM_ENABLE_BIT) == 0;
}

Protocol ProtocolManager::getActiveProtocol() const {
    if (!_dccEnabled && _mmEnabled) {
        return Protocol::MM;
    }
    if (_dccEnabled && !_mmEnabled) {
        return Protocol::DCC;
    }
    return _activeProtocol;
}

void ProtocolManager::notifyDccPacket() {
    if (_activeProtocol == Protocol::UNDECIDED && _dccEnabled) {
        _activeProtocol = Protocol::DCC;
    }
}

void ProtocolManager::notifyMmPacket() {
    if (_activeProtocol == Protocol::UNDECIDED && _mmEnabled) {
        _activeProtocol = Protocol::MM;
    }
}
