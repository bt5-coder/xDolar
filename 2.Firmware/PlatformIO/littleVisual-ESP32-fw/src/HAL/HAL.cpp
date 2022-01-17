#include "HAL/HAL.h"
#include "App/Configs/Version.h"

#define DISP_HOR_RES         CONFIG_SCREEN_HOR_RES
#define DISP_VER_RES         CONFIG_SCREEN_VER_RES
#define DISP_BUF_SIZE        CONFIG_SCREEN_BUFFER_SIZE
extern lv_color_t* lv_disp_buf_p;

void HAL::Init()
{
    // Serial.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
    Serial.begin(115200, SERIAL_8N1, 16, 17);  //Init serial port 0 for M5Stack
    Serial.println(VERSION_FIRMWARE_NAME);
    Serial.println("Version: " VERSION_SOFTWARE);
    Serial.println("Author: " VERSION_AUTHOR_NAME);

    // Move the malloc process to Init() to make sure that the largest heap can be used for this buffer.
    lv_disp_buf_p = static_cast<lv_color_t*>(malloc(DISP_BUF_SIZE * sizeof(lv_color_t)));
    if (lv_disp_buf_p == nullptr)
        LV_LOG_WARN("lv_port_disp_init malloc failed!\n");

    HAL::BT_Init(); 
    // ToDo: some of the process below will interrupt BLE connection, find it out
    // I think this is because of connection interval is not sync between master and slave
    // Need to dig it out

    //For IP5306 Status Regisitor Read Out 
    HAL::I2C_Init(true);

    HAL::Power_Init();
    HAL::Backlight_Init();
    HAL::Encoder_Init();
    HAL::Buzz_init();
    HAL::Audio_Init();
    HAL::SD_Init();

    HAL::Audio_PlayMusic("Startup");
}

void HAL::Update()
{
    // HAL::Power_Update();
    HAL::Encoder_Update();
    HAL::Audio_Update();
    HAL::BT_Update();
    __IntervalExecute(HAL::SD_Update(), 500);
}
