#if !MESHTASTIC_EXCLUDE_SNIFFER
#include "SnifferModule.h"
#include "MeshService.h"
#include "configuration.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <string>

SnifferModule *SnifferModule;

SnifferModule::SnifferModule() : SinglePortModule("sniffer", meshtastic_PortNum_TEXT_MESSAGE_APP),
                                 concurrency::OSThread("Sniffer")
{
    seenDevices = {};
}

void SnifferModule::setup()
{
    LOG_INFO("SnifferModule first time setup");
    scanner = NimBLEDevice::getScan();
    scanner->setAdvertisedDeviceCallbacks(this, false);
    scanner->setMaxResults(0);
    beginScan();
}

int32_t SnifferModule::runOnce()
{
    LOG_INFO("Sniffer generating report");
    // tx for this module disabled until further into development
    // sendReport()
    return (reportInterval);
}

void SnifferModule::beginScan()
{
    LOG_INFO("SnifferModule beginning bluetooth scan");
    scanner->start(scanTime, &SnifferModule::beginScan);
}

void SnifferModule::onResult(NimBLEAdvertisedDevice *device)
{
    LOG_INFO(("SnifferModule detected bluetooth device: " + device->toString()).c_str());
    auto ts = time(nullptr);
    auto mac = device->getAddress().toString();
    if (seenDevices.count(mac) == 1) {
        seenDevices[mac].lastSeen = ts;
        seenDevices[mac].timesSeen++;
    }
    else {
        seenDevices[mac] = DeviceInfo { mac, device->getName(), ts, ts, 0 };
    }
}

void SnifferModule::sendReport()
{
    meshtastic_MeshPacket *report = allocDataPacket();
    const char *reportMessage = "hello world :)";  // TODO
    report->decoded.payload.size = strlen(reportMessage);
    memcpy(report->decoded.payload.bytes, reportMessage, report->decoded.payload.size);
    service->sendToMesh(report);
    seenDevices.clear();
}
#endif
