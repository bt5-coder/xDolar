#ifndef __TEMPLATE_MODEL_H
#define __TEMPLATE_MODEL_H
#include "App/Utils/AccountSystem/Account.h"
#include "lvgl.h"

namespace Page
{

class TemplateModel
{
    public:
        uint32_t TickSave;
        uint32_t GetData();
        void Init();

    private:
        Account* account;
};

}

#endif
