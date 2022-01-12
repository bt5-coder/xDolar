#include "App/Accounts/Account_Master.h"
#include "TemplateModel.h"


using namespace Page;

uint32_t TemplateModel::GetData()
{
    return lv_tick_get();
}

void TemplateModel::Init()
{
    account = new Account("Template", AccountSystem::Broker(), 0, this);

    //account->SetEventCallback(onEvent);
    account->Subscribe("Encoder");
}