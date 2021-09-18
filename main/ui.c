/*
 * AWS IoT EduKit - Core2 for AWS IoT EduKit
 * Smart Thermostat v1.3.0
 * ui.c
 * 
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "core2forAWS.h"
#include "ui.h"

#define MAX_TEXTAREA_LENGTH 1024

static lv_obj_t *active_screen;
static lv_obj_t *out_txtarea;
static lv_obj_t *wifi_label;
static lv_obj_t *red_label;
static lv_obj_t* red_lmeter;
static lv_obj_t* door_label;
static lv_obj_t* mqtt_btn;
static lv_obj_t* mqtt_label;

static char *TAG = "UI";

static void mqtt_event_handler(lv_obj_t* button, lv_event_t event);

static void ui_textarea_prune(size_t new_text_length){
    const char * current_text = lv_textarea_get_text(out_txtarea);
    size_t current_text_len = strlen(current_text);
    if(current_text_len + new_text_length >= MAX_TEXTAREA_LENGTH){
        for(int i = 0; i < new_text_length; i++){
            lv_textarea_set_cursor_pos(out_txtarea, 0);
            lv_textarea_del_char_forward(out_txtarea);
        }
        lv_textarea_set_cursor_pos(out_txtarea, LV_TEXTAREA_CURSOR_LAST);
    }
}

void ui_textarea_add(char *baseTxt, char *param, size_t paramLen) {
    if( baseTxt != NULL ){
        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
        if (param != NULL && paramLen != 0){
            size_t baseTxtLen = strlen(baseTxt);
            ui_textarea_prune(paramLen);
            size_t bufLen = baseTxtLen + paramLen;
            char buf[(int) bufLen];
            sprintf(buf, baseTxt, param);
            lv_textarea_add_text(out_txtarea, buf);
        } 
        else{
            lv_textarea_add_text(out_txtarea, baseTxt); 
        }
        xSemaphoreGive(xGuiSemaphore);
    } 
    else{
        ESP_LOGE(TAG, "Textarea baseTxt is NULL!");
    }
}

void ui_wifi_label_update(bool state){
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    if (state == false) {
        lv_label_set_text(wifi_label, LV_SYMBOL_WIFI);
    } 
    else{
        char buffer[25];
        sprintf (buffer, "#0000ff %s #", LV_SYMBOL_WIFI);
        lv_label_set_text(wifi_label, buffer);
    }
    xSemaphoreGive(xGuiSemaphore);
}

void ui_meter_update(uint32_t value){
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    lv_linemeter_set_value(red_lmeter, value);
    lv_label_set_text_fmt(red_label, "CO2: %d ppm", value);
    xSemaphoreGive(xGuiSemaphore);
}

void ui_door_label_update(char *txt) {
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    lv_label_set_static_text(door_label, txt);
    xSemaphoreGive(xGuiSemaphore);
}

uint8_t ui_mqtt_get_value() {
    return lv_switch_get_state(mqtt_btn);
}

static void mqtt_event_handler(lv_obj_t* obj, lv_event_t event){
    if(event == LV_EVENT_VALUE_CHANGED) {
        uint8_t value = lv_switch_get_state(obj);

        if(value == 1) {
            lv_label_set_static_text(mqtt_label, "MQTT ON");
        } else {
            lv_label_set_static_text(mqtt_label, "MQTT OFF");
        }
        ESP_LOGI(TAG, "Vibration motor state: %x", value);
    }
}

void ui_init() {
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    active_screen = lv_scr_act();
    wifi_label = lv_label_create(active_screen, NULL);
    lv_obj_align(wifi_label,NULL,LV_ALIGN_IN_TOP_RIGHT, 0, 6);
    lv_label_set_text(wifi_label, LV_SYMBOL_WIFI);
    lv_label_set_recolor(wifi_label, true);

    out_txtarea = lv_textarea_create(active_screen, NULL);
    lv_obj_set_size(out_txtarea, 300, 100);
    lv_obj_align(out_txtarea, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);
    lv_textarea_set_max_length(out_txtarea, MAX_TEXTAREA_LENGTH);
    lv_textarea_set_text_sel(out_txtarea, false);
    lv_textarea_set_cursor_hidden(out_txtarea, true);
    lv_textarea_set_text(out_txtarea, "Starting Smart CO2 Monitor\n");

    red_lmeter = lv_linemeter_create(active_screen, NULL);
    red_label = lv_label_create(active_screen, NULL);
    lv_label_set_static_text(red_label, "CO2");
    lv_obj_align(red_label, active_screen, LV_ALIGN_IN_BOTTOM_LEFT, 20, -4);

    static lv_style_t red_lmeter_style;
    lv_style_init(&red_lmeter_style);
    lv_style_set_line_color(&red_lmeter_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_scale_grad_color(&red_lmeter_style, 40, LV_COLOR_RED);
    lv_style_set_border_opa(&red_lmeter_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&red_lmeter_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_scale_end_color(&red_lmeter_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);


    lv_linemeter_set_range(red_lmeter, 0, 3000);                   //Set the range
    lv_linemeter_set_value(red_lmeter, 3000);                       //*Set the current value
    lv_linemeter_set_scale(red_lmeter, 240, 41);                  //Set the angle and number of lines
    lv_obj_add_style(red_lmeter, LV_LINEMETER_PART_MAIN, &red_lmeter_style);
    lv_obj_set_size(red_lmeter, 120, 120);
    lv_obj_align(red_lmeter, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, 4);

    door_label = lv_label_create(active_screen, NULL);
    lv_label_set_static_text(door_label, "Door Closed");
    lv_obj_align(door_label, active_screen, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

    mqtt_label = lv_label_create(active_screen, NULL);
    lv_label_set_static_text(mqtt_label, "MQTT OFF");
    lv_obj_align(mqtt_label, active_screen, LV_ALIGN_IN_BOTTOM_MID, 100, -35);
    mqtt_btn = lv_switch_create(active_screen, NULL);
    lv_obj_set_event_cb(mqtt_btn, mqtt_event_handler);
    lv_switch_off(mqtt_btn, LV_ANIM_OFF);
    lv_obj_align(mqtt_btn, active_screen, LV_ALIGN_IN_BOTTOM_MID, 30, -30);


    xSemaphoreGive(xGuiSemaphore);
}