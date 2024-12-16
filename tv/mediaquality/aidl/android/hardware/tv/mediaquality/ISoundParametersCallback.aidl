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

package android.hardware.tv.mediaquality;

import android.hardware.tv.mediaquality.SoundParameters;

@VintfStability
interface ISoundParametersCallback {
    /**
     * Get the Sound parameters by SoundProfile id. Check SoundParameters for its' detail.
     * This is called from the HAL to media quality framework.
     *
     * @param SoundProfileId The SoundProfile id that associate with the SoundProfile.
     * @return SoundParameters with all the pre-defined parameters and vendor defined parameters.
     */
    SoundParameters getSoundParameters(long SoundProfileId);
}
