#include "HAL/HAL.h"
#include "NimBLEDevice.h"
#include "App/Accounts/Account_Master.h"

/* BLE Scan Result record */
typedef struct 
{
    const char* name;
    const char* addr;
    int ssid;
} Scan_Info_t;
static Scan_Info_t scan_info[5];

void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
    std::string str = isNotify ? "Notification" : "Indication";
    str += " from ";
    str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
    str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
    str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
    str += ", Value = ";
    Serial.print(str.c_str());
    Serial.println(pRemoteCharacteristic->getValue<float>());
}

void scanEndedCB(NimBLEScanResults results) 
{
    Serial.println("Scan Ended");
}

static NimBLEAdvertisedDevice* advDevice;
static bool doConnect = false;
static bool isConnected = false;

class ClientCallbacks : public NimBLEClientCallbacks
{
    void onConnect(NimBLEClient* pClient)
    {
        Serial.println("Connected");
        pClient->updateConnParams(120, 120, 0, 300);
        isConnected = true;
    };

    void onDisconnect(NimBLEClient* pClient)
    {
        Serial.print(pClient->getPeerAddress().toString().c_str());
        Serial.println(" Disconnected - Starting scan");
        NimBLEDevice::getScan()->start(0, scanEndedCB);
        isConnected = false;
    };

    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params)
    {
        if (params->itvl_min < 24)
        { /** 1.25ms units */
            return false;
        } else if (params->itvl_max > 40)
        { /** 1.25ms units */
            return false;
        } else if (params->latency > 2)
        { /** Number of intervals allowed to skip */
            return false;
        } else if (params->supervision_timeout > 100)
        { /** 10ms units */
            return false;
        }

        return true;
    };

    uint32_t onPassKeyRequest()
    {
        Serial.println("Client Passkey Request");
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key)
    {
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        return true;
    };

    void onAuthenticationComplete(ble_gap_conn_desc* desc)
    {
        if (!desc->sec_state.encrypted)
        {
            Serial.println("Encrypt connection failed - disconnecting");
            NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
            return;
        }
    };
};

class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
    void onResult(NimBLEAdvertisedDevice* advertisedDevice)
    {

        HAL::BLE_Info_t bleInfo;
        static int8_t topfive_cnt;
        //storage scan data in a static buf
        static char buf_name[5][16],buf_addr[5][20]; 
        int8_t i;
        Serial.print("Advertised Device found: ");
        //Serial.println(advertisedDevice->toString().c_str());
        Serial.println(advertisedDevice->getName().c_str() );
        Serial.println(advertisedDevice->getAddress().toString().c_str() );
        Serial.println(advertisedDevice->getRSSI() );

        memcpy(buf_name[topfive_cnt], advertisedDevice->getName().c_str(),15);
        strcpy(buf_addr[topfive_cnt], advertisedDevice->getAddress().toString().c_str());

        // log top five data
        scan_info[topfive_cnt].name = buf_name[topfive_cnt];
        scan_info[topfive_cnt].addr = buf_addr[topfive_cnt];
        scan_info[topfive_cnt].ssid = advertisedDevice->getRSSI();
        topfive_cnt ++;
        if (topfive_cnt >=5)
            topfive_cnt = 0;
            
        //if (advertisedDevice->isAdvertisingService(NimBLEUUID("0001")))
        if (advertisedDevice->getAddress() == NimBLEAddress("50:02:91:ac:b0:6a"))
        {
            Serial.println("Found SensorTag");
            NimBLEDevice::getScan()->stop();
            advDevice = advertisedDevice;
            doConnect = true;
        }
    };
};

static ClientCallbacks clientCB;

bool connectToServer()
{
    NimBLEClient* pClient = nullptr;

    if (NimBLEDevice::getClientListSize())
    {
        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
        if (pClient)
        {
            if (!pClient->connect(advDevice, false))
            {
                Serial.println("Reconnect failed");
                return false;
            }
            Serial.println("Reconnected client");
        }
        else
        {
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    if (!pClient)
    {
        if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS)
        {
            Serial.println("Max clients reached - no more connections available");
            return false;
        }

        pClient = NimBLEDevice::createClient();

        Serial.println("New client created");

        pClient->setClientCallbacks(&clientCB, false);
        pClient->setConnectionParams(12, 12, 0, 51);
        pClient->setConnectTimeout(5);

        if (!pClient->connect(advDevice))
        {
            NimBLEDevice::deleteClient(pClient);
            Serial.println("Failed to connect, deleted client");
            return false;
        }
    }

    if (!pClient->isConnected())
    {
        if (!pClient->connect(advDevice))
        {
            Serial.println("Failed to connect");
            return false;
        }
    }

    Serial.print("Connected to: ");
    Serial.println(pClient->getPeerAddress().toString().c_str());
    Serial.print("RSSI: ");
    Serial.println(pClient->getRssi());

    NimBLERemoteService* pSvc = nullptr;
    NimBLERemoteCharacteristic* pChr = nullptr;
    NimBLERemoteDescriptor* pDsc = nullptr;

    pSvc = pClient->getService("0001");
    if (pSvc)
    {     /** make sure it's not null */
        pChr = pSvc->getCharacteristic("1001");

        if (pChr)
        {     /** make sure it's not null */
            if (pChr->canRead())
            {
                Serial.print(pChr->getUUID().toString().c_str());
                Serial.print(" Value: ");
                Serial.println(*(float*) (pChr->readValue().c_str()));
            }

//            if (pChr->canWrite())
//            {
//                if (pChr->writeValue("Tasty"))
//                {
//                    Serial.print("Wrote new value to: ");
//                    Serial.println(pChr->getUUID().toString().c_str());
//                } else
//                {
//                    /** Disconnect if write failed */
//                    pClient->disconnect();
//                    return false;
//                }
//
//                if (pChr->canRead())
//                {
//                    Serial.print("The value of: ");
//                    Serial.print(pChr->getUUID().toString().c_str());
//                    Serial.print(" is now: ");
//                    Serial.println(pChr->readValue().c_str());
//                }
//            }

            /** registerForNotify() has been deprecated and replaced with subscribe() / unsubscribe().
             *  Subscribe parameter defaults are: notifications=true, notifyCallback=nullptr, response=false.
             *  Unsubscribe parameter defaults are: response=false.
             */
            if (pChr->canNotify())
            {
                //if(!pChr->registerForNotify(notifyCB)) {
                if (!pChr->subscribe(true, notifyCB))
                {
                    /** Disconnect if subscribe failed */
                    pClient->disconnect();
                    return false;
                }
            }
//            else if (pChr->canIndicate())
//            {
//                /** Send false as first argument to subscribe to indications instead of notifications */
//                //if(!pChr->registerForNotify(notifyCB, false)) {
//                if (!pChr->subscribe(false, notifyCB))
//                {
//                    /** Disconnect if subscribe failed */
//                    pClient->disconnect();
//                    return false;
//                }
//            }
        }

    } else
    {
        Serial.println("0001 service not found.");
    }

    Serial.println("Done with this device!");
    return true;
}


bool HAL::BT_Init()
{
    Serial.println("Starting xDolar Client");
    NimBLEDevice::init("xDolar");

    NimBLEDevice::setSecurityAuth(false, false, true);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */

    /** create new scan */
    NimBLEScan* pScan = NimBLEDevice::getScan();
    /** create a callback that gets called when advertisers are found */
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    /** Set scan interval (how often) and window (how long) in milliseconds */
    pScan->setInterval(300);
    pScan->setWindow(200);
    pScan->setActiveScan(true);
    pScan->start(0, scanEndedCB);   //Always scan

    return true;
}

void HAL::BT_Update()
{
    BLE_Info_t bleInfo;

    if (doConnect)
    {
        doConnect = false;

        /** Found a device we want to connect to, do it now */
        if (connectToServer())
        {
            Serial.println("Success! we should now be getting notifications, scanning for more!");
        } else
        {
            Serial.println("Failed to connect, starting scan");
            NimBLEDevice::getScan()->start(0, scanEndedCB);
        }
    }
    else
    {
        bleInfo.dev1 = scan_info[0].name; 
        bleInfo.dev2 = scan_info[1].name; 
        bleInfo.dev3 = scan_info[2].name; 
        bleInfo.dev4 = scan_info[3].name; 
        bleInfo.dev5 = scan_info[4].name; 

        AccountSystem::BLE_Commit(&bleInfo); 
    }


}

bool HAL::BluetoothConnected()
{
    return isConnected;
}
