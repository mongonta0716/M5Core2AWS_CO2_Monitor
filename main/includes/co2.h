/*
 * AWS IoT EduKit - Core2 for AWS IoT EduKit
 * Smart Thermostat v1.3.0
 * wifi.h
 * 
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#pragma once
#ifndef _MHZ19_CO2_H_
#define _MHZ19_CO2_H_
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
extern "C" {
#include "mhz19.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */

/* The event group allows multiple bits for each event */
void start_mhz19(void);
}
#endif // _CO2_H_