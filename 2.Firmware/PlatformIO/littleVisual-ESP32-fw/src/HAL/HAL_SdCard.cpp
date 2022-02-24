#include "HAL/HAL.h"
#include "SPI.h"
#include "SD.h"

static bool SD_IsReady = false;
static uint64_t SD_CardSize = 0;
static uint64_t SD_FreeSize = 0;
static HAL::SD_CallbackFunction_t SD_EventCallback = nullptr;

bool HAL::SD_Init()
{
    pinMode(CONFIG_SD_DET_PIN, INPUT);
    pinMode(CONFIG_SD_CS_PIN, OUTPUT);

    SPIClass* sd_spi = new SPIClass(SPI_SD); // another SPI
    if (!SD.begin(CONFIG_SD_CS_PIN, *sd_spi, 80000000)) // SD-Card SS pin is 4
    if (!SD.begin()) // SD-Card SS pin is 4
    {
        Serial.println("Card Mount Failed");
        return false;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return false;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    } else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    } else
    {
        Serial.println("UNKNOWN");
    }

    SD_CardSize = SD.cardSize() / (1024 * 1024);
    SD_FreeSize = ( SD.totalBytes() - SD.usedBytes() ) / 1024 / 1024;
    Serial.printf("SD Card Size: %0.0fMB\n", float (SD_CardSize) );
    Serial.printf("SD Card Unused Size: %lluMB\n", SD_FreeSize);
    SD_IsReady = true;
    return true;
}

bool HAL::SD_GetReady()
{
    return (SD_IsReady);
}

float HAL::SD_GetCardSizeMB()
{
    return (float (SD_CardSize));
}

float HAL::SD_GetFreeSizeMB()
{
    return (float (SD_FreeSize));
}

static void SD_Check(bool isInsert)
{
    if(isInsert)
    {
        bool ret = HAL::SD_Init();

        if(ret && SD_EventCallback)
        {
            SD_EventCallback(true);
        }

        HAL::Audio_PlayMusic(ret ? "DeviceInsert" : "Error");
    }
    else
    {
        SD_IsReady = false;

        if(SD_EventCallback)
        {
            SD_EventCallback(false);
            SD_CardSize = 0;
        }

        HAL::Audio_PlayMusic("DevicePullout");
    }
}

void HAL::SD_SetEventCallback(SD_CallbackFunction_t callback)
{
    SD_EventCallback = callback;
}

void HAL::SD_Update()
{
    bool isInsert = SD_IsReady;
    __ValueMonitor(isInsert, SD_Check(isInsert));
}
