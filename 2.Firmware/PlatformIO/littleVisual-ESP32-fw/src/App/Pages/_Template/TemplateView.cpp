#include "TemplateView.h"

using namespace Page;

void TemplateView::Create(lv_obj_t* root)
{

	lv_obj_t* bgk = lv_obj_create(root);
	lv_obj_set_style_bg_opa(bgk, LV_OPA_100, LV_PART_MAIN);
	lv_obj_set_style_bg_color(bgk, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_size(bgk, 240, 320);	//	Screen Size 

	lv_obj_t* label = lv_label_create(root);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
	lv_obj_set_style_text_color(label, lv_color_white(), 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);
	lv_label_set_text(label, "");
	ui.labelTitle = label;

	label = lv_label_create(root);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
	lv_obj_set_style_text_color(label, lv_color_white(), 0);
	lv_label_set_text(label, "");
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 50);
	ui.labelTick = label;

	lv_obj_t* img = lv_img_create(root);
	lv_obj_set_style_bg_color(img, lv_color_black(), LV_PART_MAIN);
	lv_img_set_src(img, Resource.GetImage("blob"));
	lv_obj_center(img);
	lv_obj_align(img, LV_ALIGN_BOTTOM_MID, 0, 0);
	ui.canvas = img;

	ui.group = lv_group_create();
	lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), ui.group);

	lv_group_add_obj(ui.group, ui.canvas);
	lv_group_add_obj(ui.group, ui.labelTitle);
	lv_group_focus_obj(ui.canvas);

	// lv_group_set_editing(ui.group,false);
}

void TemplateView::Delete()
{
	lv_group_del(ui.group);
	// Style_Reset();
}