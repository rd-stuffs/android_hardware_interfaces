/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.boot@1.0-service"

#include <android/hardware/boot/1.0/IBootControl.h>
#include <hidl/LegacySupport.h>
#include <hwbinder/ProcessState.h>

using ::android::hardware::boot::V1_0::IBootControl;
using android::hardware::defaultPassthroughServiceImplementation;

int main (int /* argc */, char * /* argv */ []) {
// QTI_BEGIN: 2018-09-26: Core: Tuning of Binder buffer for below HALs on 32-bit
#ifdef ARCH_ARM_32
// QTI_END: 2018-09-26: Core: Tuning of Binder buffer for below HALs on 32-bit
// QTI_BEGIN: 2018-07-10: Core: Tuning of Binder buffer for below HALs
    android::hardware::ProcessState::initWithMmapSize((size_t)8192);
// QTI_END: 2018-07-10: Core: Tuning of Binder buffer for below HALs
// QTI_BEGIN: 2018-09-26: Core: Tuning of Binder buffer for below HALs on 32-bit
#endif
// QTI_END: 2018-09-26: Core: Tuning of Binder buffer for below HALs on 32-bit
    return defaultPassthroughServiceImplementation<IBootControl>();
}
