#pragma once
#if !MESHTASTIC_EXCLUDE_SNIFFER
#include "SinglePortModule.h"
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
#include <map>
#include <string>

struct DeviceInfo {
    std::string mac;
    std::string name;
    time_t firstSeen;
    time_t lastSeen;
    unsigned int timesSeen;
};

/**
 * A module that sniffs local bluetooth traffic and reports devices over mesh.
 * This intends to replicate the functionality of the [MeshDetect project](https://www.hackster.io/colonelpanic/mesh-detect-549cbe),
 * from within the Meshtastic firmware. Traffic sniffing results are sent over the mesh on a custom port number.
 *
 * Note that this solution is not resistant to thid parties eavesdropping on the sniffing results. I may implement
 * sending device reports over encrypted channels in the future, but the unattended nodes would still be vulnerable
 * to physical tampering to retrieve the channel PSK.
 *
 * Furthermore, care should be taken to ensure this module does not result in excessive transmissions, particularly
 * in urban areas. Bluetooth and wifi devices are everywhere, and without proper configuration this module will
 * report on all of them. I strongly recomend measures such as picking a long report interval, and/or filtering
 * for specific OUIs of interest to you.
 */
class SnifferModule : public SinglePortModule, private concurrency::OSThread, public NimBLEAdvertisedDeviceCallbacks
{
  public:
    SnifferModule();
    virtual int32_t runOnce() override;
    virtual void onResult(NimBLEAdvertisedDevice *device);
    void beginScan();

  protected:
    virtual void setup() override;

  private:
    static const int portNumber = meshtastic_PortNum_PRIVATE_APP;
    bool firstTime = 1;
    NimBLEScan *scanner;
    std::map<std::string, DeviceInfo> seenDevices;
    unsigned int reportInterval;
    unsigned int scanTime = 30 * 1000;  // in milis
    void sendReport();
};

extern SnifferModule *snifferModule;
#endif
