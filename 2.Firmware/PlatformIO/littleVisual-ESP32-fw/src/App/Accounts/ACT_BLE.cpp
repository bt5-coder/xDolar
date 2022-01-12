#include "Account_Master.h"
#include "HAL/HAL.h"

static Account* actBLE;

void AccountSystem::BLE_Commit(HAL::BLE_Info_t* info)
{
    actBLE->Commit(info, sizeof(HAL::BLE_Info_t));
}

ACCOUNT_INIT_DEF(BLE)
{
    actBLE = account;
}