/*
 * Copyright (C) 2022 The Android Open Source Project
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

#define LOG_TAG "VtsHalBassBoostTest"

#include <Utils.h>
#include <aidl/Vintf.h>
#include <limits.h>

#include "EffectHelper.h"

using namespace android;

using aidl::android::hardware::audio::effect::BassBoost;
using aidl::android::hardware::audio::effect::Capability;
using aidl::android::hardware::audio::effect::Descriptor;
using aidl::android::hardware::audio::effect::IEffect;
using aidl::android::hardware::audio::effect::IFactory;
using aidl::android::hardware::audio::effect::kBassBoostTypeUUID;
using aidl::android::hardware::audio::effect::Parameter;

/**
 * Here we focus on specific parameter checking, general IEffect interfaces testing performed in
 * VtsAudioEffectTargetTest.
 */
enum ParamName { PARAM_INSTANCE_NAME, PARAM_STRENGTH };
using BassBoostParamTestParam = std::tuple<std::pair<std::shared_ptr<IFactory>, Descriptor>, int>;

/*
 * Testing parameter range, assuming the parameter supported by effect is in this range.
 * Parameter should be within the valid range defined in the documentation,
 * for any supported value test expects EX_NONE from IEffect.setParameter(),
 * otherwise expect EX_ILLEGAL_ARGUMENT.
 */

const std::vector<int> kStrengthValues = {
        std::numeric_limits<int>::min(),
        BassBoost::MIN_PER_MILLE_STRENGTH - 1,
        BassBoost::MIN_PER_MILLE_STRENGTH,
        (BassBoost::MIN_PER_MILLE_STRENGTH + BassBoost::MAX_PER_MILLE_STRENGTH) >> 1,
        BassBoost::MAX_PER_MILLE_STRENGTH,
        BassBoost::MAX_PER_MILLE_STRENGTH + 2,
        std::numeric_limits<int>::max()};

class BassBoostParamTest : public ::testing::TestWithParam<BassBoostParamTestParam>,
                           public EffectHelper {
  public:
    BassBoostParamTest() : mParamStrength(std::get<PARAM_STRENGTH>(GetParam())) {
        std::tie(mFactory, mDescriptor) = std::get<PARAM_INSTANCE_NAME>(GetParam());
    }

    void SetUp() override {
        ASSERT_NE(nullptr, mFactory);
        ASSERT_NO_FATAL_FAILURE(create(mFactory, mEffect, mDescriptor));

        Parameter::Specific specific = getDefaultParamSpecific();
        Parameter::Common common = EffectHelper::createParamCommon(
                0 /* session */, 1 /* ioHandle */, 44100 /* iSampleRate */, 44100 /* oSampleRate */,
                kInputFrameCount /* iFrameCount */, kOutputFrameCount /* oFrameCount */);
        IEffect::OpenEffectReturn ret;
        ASSERT_NO_FATAL_FAILURE(open(mEffect, common, specific, &ret, EX_NONE));
        ASSERT_NE(nullptr, mEffect);
    }

    void TearDown() override {
        ASSERT_NO_FATAL_FAILURE(close(mEffect));
        ASSERT_NO_FATAL_FAILURE(destroy(mFactory, mEffect));
    }

    Parameter::Specific getDefaultParamSpecific() {
        BassBoost bb = BassBoost::make<BassBoost::strengthPm>(BassBoost::MIN_PER_MILLE_STRENGTH);
        Parameter::Specific specific =
                Parameter::Specific::make<Parameter::Specific::bassBoost>(bb);
        return specific;
    }

    static const long kInputFrameCount = 0x100, kOutputFrameCount = 0x100;
    std::shared_ptr<IFactory> mFactory;
    std::shared_ptr<IEffect> mEffect;
    Descriptor mDescriptor;
    int mParamStrength = BassBoost::MIN_PER_MILLE_STRENGTH;

    void SetAndGetBassBoostParameters() {
        for (auto& it : mTags) {
            auto& tag = it.first;
            auto& bb = it.second;

            // validate parameter
            Descriptor desc;
            ASSERT_STATUS(EX_NONE, mEffect->getDescriptor(&desc));
            const bool valid = isTagInRange(it.first, it.second, desc);
            const binder_exception_t expected = valid ? EX_NONE : EX_ILLEGAL_ARGUMENT;

            // set parameter
            Parameter expectParam;
            Parameter::Specific specific;
            specific.set<Parameter::Specific::bassBoost>(bb);
            expectParam.set<Parameter::specific>(specific);
            EXPECT_STATUS(expected, mEffect->setParameter(expectParam)) << expectParam.toString();

            // only get if parameter in range and set success
            if (expected == EX_NONE) {
                Parameter getParam;
                Parameter::Id id;
                BassBoost::Id bbId;
                bbId.set<BassBoost::Id::commonTag>(tag);
                id.set<Parameter::Id::bassBoostTag>(bbId);
                // if set success, then get should match
                EXPECT_STATUS(expected, mEffect->getParameter(id, &getParam));
                EXPECT_EQ(expectParam, getParam);
            }
        }
    }

    void addStrengthParam(int strength) {
        BassBoost bb;
        bb.set<BassBoost::strengthPm>(strength);
        mTags.push_back({BassBoost::strengthPm, bb});
    }

    bool isTagInRange(const BassBoost::Tag& tag, const BassBoost& bb,
                      const Descriptor& desc) const {
        const BassBoost::Capability& bbCap = desc.capability.get<Capability::bassBoost>();
        switch (tag) {
            case BassBoost::strengthPm: {
                int strength = bb.get<BassBoost::strengthPm>();
                return isStrengthInRange(bbCap, strength);
            }
            default:
                return false;
        }
        return false;
    }

    bool isStrengthInRange(const BassBoost::Capability& cap, int strength) const {
        return cap.strengthSupported && strength >= BassBoost::MIN_PER_MILLE_STRENGTH &&
               strength <= BassBoost::MAX_PER_MILLE_STRENGTH;
    }

  private:
    std::vector<std::pair<BassBoost::Tag, BassBoost>> mTags;
    void CleanUp() { mTags.clear(); }
};

TEST_P(BassBoostParamTest, SetAndGetStrength) {
    EXPECT_NO_FATAL_FAILURE(addStrengthParam(mParamStrength));
    SetAndGetBassBoostParameters();
}

INSTANTIATE_TEST_SUITE_P(
        BassBoostTest, BassBoostParamTest,
        ::testing::Combine(testing::ValuesIn(EffectFactoryHelper::getAllEffectDescriptors(
                                   IFactory::descriptor, kBassBoostTypeUUID)),
                           testing::ValuesIn(kStrengthValues)),
        [](const testing::TestParamInfo<BassBoostParamTest::ParamType>& info) {
            auto descriptor = std::get<PARAM_INSTANCE_NAME>(info.param).second;
            std::string strength = std::to_string(std::get<PARAM_STRENGTH>(info.param));
            std::string name = "Implementor_" + descriptor.common.implementor + "_name_" +
                               descriptor.common.name + "_UUID_" +
                               descriptor.common.id.uuid.toString() + "_strength_" + strength;
            std::replace_if(
                    name.begin(), name.end(), [](const char c) { return !std::isalnum(c); }, '_');
            return name;
        });

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(BassBoostParamTest);

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();
    return RUN_ALL_TESTS();
}
