/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "HAL/HAL.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_indev_t* encoder_indev;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static lv_indev_drv_t indev_drv;

void lv_port_indev_init(void)
{

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    encoder_indev = lv_indev_drv_register(&indev_drv);
    lv_indev_t* indev = lv_indev_drv_register(&indev_drv);
    
    lv_group_t* group = lv_group_create();
    lv_indev_set_group(indev, group);
    lv_group_set_default(group);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_encoder, group);` */
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Encoder
 * -----------------*/

/* Initialize your keypad */
static void encoder_init(void)
{
    /*Do it in Init*/
}


/* Will be called by the library to read the encoder */
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    //data->enc_diff = HAL::Encoder_GetDiff();
    uint16_t last_key = 0;
    static uint16_t act_key;
    // Button emulations 
    // data->enc_diff 
    act_key = HAL::Encoder_GetEmulatorButton();
    if(act_key != 0) 
    {
        
        switch(act_key) 
        {
            case 1:
                //act_key = 1;
                data->enc_diff = LV_KEY_NEXT ;
                // Serial.println("last_key :  1 ");
                break;
            case 2:
                //act_key = -1;
                data->enc_diff = LV_KEY_PREV  ;
                // Serial.println("last_key :  -1 ");
                break;
        }
        last_key = act_key;
    }

    // data->enc_diff = last_key;

    data->state =  HAL::Encoder_GetIsPush() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif