/*
 * Copyright (C) 2024 The Android Open Source Project
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

package android.hardware.vibrator;

import android.hardware.vibrator.EffectStrength;
import android.os.PersistableBundle;

@VintfStability
parcelable VendorEffect {
    /**
     * Vendor data describing the haptic effect. Expected fields should be defined by the vendor.
     *
     * Vendors can use this as a platform extension point for experimental hardware capabilities,
     * but they are strongly discouraged from using it as an alternative to the AOSP support for
     * stable vibrator APIs. Implemenitng vendor-specific custom effects outside the platform APIs
     * will hinder portability for the code and overall user experience.
     *
     * Vendors are encouraged to upstream new capabilities to the IVibrator surface once it has
     * matured into a stable interface.
     */
    PersistableBundle vendorData;

    /**
     * The intensity of the haptic effect.
     */
    EffectStrength strength = EffectStrength.MEDIUM;

    /**
     * A scale to be applied to the haptic effect intensity.
     *
     * This value represents a linear scale that should be applied on top of the effect strength to
     * dynamically adapt to the device state.
     *
     * Values in [0,1) should scale down. Values > 1 should scale up within hardware bounds.
     */
    float scale;
}
