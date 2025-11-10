#ifndef CV_MANAGER_ADAPTER_H
#define CV_MANAGER_ADAPTER_H

#include "AuxController.h"
#include "CVManager.h"

class CVManagerAdapter : public xDuinoRails::ICVAccess {
public:
    CVManagerAdapter(CVManager& cvManager) : _cvManager(cvManager) {}

    uint8_t readCV(uint16_t cv_number) override {
        return _cvManager.readCV(cv_number);
    }

    void writeCV(uint16_t cv_number, uint8_t value) override {
        _cvManager.writeCV(cv_number, value);
    }

private:
    CVManager& _cvManager;
};

#endif // CV_MANAGER_ADAPTER_H
