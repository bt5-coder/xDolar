#include "Scene3D.h"
#include "lvgl.h"
#include "HAL/HAL.h"

using namespace Page;

Scene3D::Scene3D()
{
}

Scene3D::~Scene3D()
{

}

void Scene3D::onCustomAttrConfig()
{
    SetCustomCacheEnable(true);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_BOTTOM, 500, lv_anim_path_bounce);
}


void Scene3D::onViewLoad()
{
    Model.Init();
    View.Create(root);

    //AttachEvent(root);
    AttachEvent(View.ui.canvas);
}

void Scene3D::onViewDidLoad()
{

}

void Scene3D::onViewWillAppear()
{
    // first time on view load, it will added button control and display
    // but after push and pop, recover to this page
    // you need to sync button control to this display
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), View.ui.group); 

    timer = lv_timer_create(onTimerUpdate, 20, this);
    lv_timer_ready(timer);
}

void Scene3D::onViewDidAppear()
{

}

void Scene3D::onViewWillDisappear()
{

}

void Scene3D::onViewDidDisappear()
{
    lv_group_t* group = lv_group_get_default();
    lv_group_remove_all_objs(group);
    lv_timer_del(timer);
}

void Scene3D::onViewDidUnload()
{
    View.Delete();
}

void Scene3D::AttachEvent(lv_obj_t* obj)
{
    lv_obj_set_user_data(obj, this);
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_PRESSED, this);
}

void Scene3D::Update()
{
     Model.Update(View.ui.canvas);
}

void Scene3D::onTimerUpdate(lv_timer_t* timer)
{
    Scene3D* instance = (Scene3D*) timer->user_data;

    instance->Update();
}

void Scene3D::onEvent(lv_event_t* event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    auto* instance = (Scene3D*) lv_obj_get_user_data(obj);

    Serial.println("Scene3D onEvent tiggered");

    if (obj == instance->View.ui.canvas)
    {
        if (code == LV_EVENT_PRESSED || code == LV_EVENT_LEAVE)
        {
            Serial.println("Scene3D key tiggered");
            // if (lv_obj_has_state(obj, LV_STATE_FOCUSED))
            {    
                instance->Manager->Pop();
            }

        }
    }
    
}
