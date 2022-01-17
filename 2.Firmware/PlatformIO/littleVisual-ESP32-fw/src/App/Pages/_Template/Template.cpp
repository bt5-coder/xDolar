#include "Template.h"
#include "HAL/HAL.h"

using namespace Page;

Template::Template()
{
}

Template::~Template()
{

}

void Template::onCustomAttrConfig()
{
	SetCustomCacheEnable(true);
	SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_BOTTOM, 500, lv_anim_path_bounce);
}

void Template::onViewLoad()
{
	Model.Init();
	View.Create(root);
	lv_label_set_text(View.ui.labelTitle, Name);

	//AttachEvent(root);  // can be ignore
	AttachEvent(View.ui.canvas);

	Model.TickSave = Model.GetData();
}

void Template::onViewDidLoad()
{
    //lv_obj_set_user_data(View.ui.canvas, this);
    //lv_obj_add_event_cb(View.ui.canvas, onEvent, LV_EVENT_VALUE_CHANGED, this);
}

void Template::onViewWillAppear()
{
	// first time on view load, it will added button control and display
    // but after push and pop, recover to this page
    // you need to sync button control to this display
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), View.ui.group); 

	Param_t param;
	param.color = lv_color_white();
	param.time = 1000;

	PAGE_STASH_POP(param);

	lv_obj_set_style_bg_color(root, param.color, LV_PART_MAIN);

	timer = lv_timer_create(onTimerUpdate, param.time, this);
}

void Template::onViewDidAppear()
{

}

void Template::onViewWillDisappear()
{

}

void Template::onViewDidDisappear()
{
	lv_timer_del(timer);
}

void Template::onViewDidUnload()
{
    View.Delete();
}

void Template::AttachEvent(lv_obj_t* obj)
{
	lv_obj_set_user_data(obj, this);
	lv_obj_add_event_cb(obj, onEvent, LV_EVENT_PRESSED, this);
}

void Template::Update()
{
	lv_label_set_text_fmt(View.ui.labelTick, "tick = %d save = %d", Model.GetData(), Model.TickSave);
}

void Template::onTimerUpdate(lv_timer_t* timer)
{
	Template* instance = (Template*)timer->user_data;

	instance->Update();
}

void Template::onEvent(lv_event_t* event)
{
	lv_obj_t* obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto* instance = (Template*)lv_obj_get_user_data(obj);

    Serial.println("Template onEvent tiggered");

	if (obj == instance->View.ui.canvas)
	{
		if (code == LV_EVENT_PRESSED || code == LV_EVENT_LEAVE)
		{
			// if (lv_obj_has_state(obj, LV_STATE_FOCUSED))
			{
				//instance->Manager->Push("Pages/Scene3D");
				Serial.println("Template Button tiggered");
				instance->Manager->Push("Pages/SystemInfos");
			}
		}
	}
}


