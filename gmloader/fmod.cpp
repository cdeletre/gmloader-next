#if BUILD_FMOD
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"
#include "configuration.h"
#include "fmod.hpp"
#include "fmod_studio.hpp"

FMOD::System* fm_system = nullptr;
FMOD::Studio::System* fms_studioSystem = nullptr;
FMOD::Studio::EventInstance* fms_event_instance = nullptr;
std::vector<FMOD::Studio::EventInstance*> fms_event_instances;


ABI_ATTR void fmod_init(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int maxchannels = YYGetReal(args, 0);
    FMOD::System_Create(&fm_system);
    fm_system->init(maxchannels, FMOD_INIT_NORMAL, 0);
    FMOD::Studio::System::create(&fms_studioSystem);
    fms_studioSystem->initialize(maxchannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_destroy(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fm_system)
    {
        fms_studioSystem->release();
        fms_studioSystem = nullptr;
    }
    if (fm_system)
    {
        fm_system->release();
        fm_system = nullptr;
    }
    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_bank_load(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char *bank_name = YYGetString(args, 0);
    std::string fmod_banks = gmloader_config.fmod_bank_path;
    std::string bank_path = fmod_banks + bank_name;

    FMOD::Studio::Bank* bank;
    FMOD_RESULT result = fms_studioSystem->loadBankFile(bank_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);

    if (result != FMOD_OK)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_update(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_studioSystem)
    {
        fms_studioSystem->update();
        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_create_instance(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char *event_path = YYGetString(args, 0);
    FMOD::Studio::EventDescription* event_description = nullptr;

    FMOD_RESULT result = fms_studioSystem->getEvent(event_path, &event_description);
    if (result != FMOD_OK || event_description == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    result = event_description->createInstance(&fms_event_instance);
    if (result != FMOD_OK || fms_event_instance == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    fms_event_instances.push_back(fms_event_instance);
    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_event_instance_play(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        fms_event_instance->start();
        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_stop(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        fms_event_instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_release(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        fms_event_instance->release();
        fms_event_instance = nullptr;
        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_set_3d_attributes(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        float posX = YYGetReal(args, 1);
        float posY = YYGetReal(args, 2);
        float velX = 0.0f;
        float velY = 0.0f;

        FMOD_3D_ATTRIBUTES attributes = { { posX, posY, 0.0f }, { velX, velY, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } };
        fms_event_instance->set3DAttributes(&attributes);

        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_set_listener_attributes(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int listener_index = YYGetInt32(args, 0);
    float posX = YYGetReal(args, 1);
    float posY = YYGetReal(args, 2);
    float velX = 0.0f;
    float velY = 0.0f;

    FMOD_3D_ATTRIBUTES attributes = { { posX, posY, 0.0f }, { velX, velY, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } };
    fms_studioSystem->setListenerAttributes(listener_index, &attributes);

    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_set_num_listeners(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int num_listeners = YYGetInt32(args, 0);

    FMOD_RESULT result = fms_studioSystem->setNumListeners(num_listeners);
    if (result == FMOD_OK)
    {
        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}   

ABI_ATTR void fmod_event_instance_set_parameter(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        char *parameter_name = YYGetString(args, 0);
        float value = YYGetReal(args, 2);

        FMOD_RESULT result = fms_event_instance->setParameterByName(parameter_name, value);
        if (result == FMOD_OK)
        {
            ret->rvalue.val = 1.0;
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_get_parameter(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        char *parameter_name = YYGetString(args, 0);
        float value;

        FMOD_RESULT result = fms_event_instance->getParameterByName(parameter_name, &value);
        if (result == FMOD_OK)
        {
            ret->rvalue.val = value;
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_set_parameter(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char *parameter_name = YYGetString(args, 0);
    float value = YYGetReal(args, 1);

    FMOD_RESULT result = fms_studioSystem->setParameterByName(parameter_name, value);
    if (result == FMOD_OK)
    {
        ret->rvalue.val = 1.0;
    }
    else
    {
        ret->rvalue.val = 0.0; 
    }

    ret->kind = VALUE_REAL;
}

ABI_ATTR void fmod_get_parameter(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char *parameter_name = YYGetString(args, 0);
    float value;

    FMOD_RESULT result = fms_studioSystem->getParameterByName(parameter_name, &value);
    if (result == FMOD_OK)
    {
        ret->rvalue.val = value;
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_set_paused(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        int flag = YYGetInt32(args, 1);

        FMOD_RESULT result = fms_event_instance->setPaused(flag);
        if (result == FMOD_OK)
        {
            ret->rvalue.val = 1.0;
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_get_paused(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        bool flag;
        FMOD_RESULT result = fms_event_instance->getPaused(&flag);
        if (result == FMOD_OK)
        {
            ret->rvalue.val = flag ? 1.0 : 0.0;
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_set_paused_all(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int flag = YYGetInt32(args, 0);

    for (auto& instance : fms_event_instances)
    {
        if (instance)
        {
            instance->setPaused(flag);
        }
    }

    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_event_one_shot(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char *event_path = YYGetString(args, 0);
    FMOD::Studio::EventDescription* event_description = nullptr;
    FMOD::Studio::EventInstance* one_shot_instance = nullptr;

    FMOD_RESULT result = fms_studioSystem->getEvent(event_path, &event_description);
    if (result != FMOD_OK || event_description == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    result = event_description->createInstance(&one_shot_instance);
    if (result != FMOD_OK || one_shot_instance == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    result = one_shot_instance->start();
    if (result != FMOD_OK)
    {
        one_shot_instance->release();
        ret->rvalue.val = 0.0;
        return;
    }

    one_shot_instance->release();
    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_event_one_shot_3d(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char *event_path = YYGetString(args, 0);
    float posX = YYGetReal(args, 1);
    float posY = YYGetReal(args, 2);
    float velX = 0.0f;
    float velY = 0.0f;

    FMOD::Studio::EventDescription* event_description = nullptr;
    FMOD::Studio::EventInstance* one_shot_instance = nullptr;

    FMOD_RESULT result = fms_studioSystem->getEvent(event_path, &event_description);
    if (result != FMOD_OK || event_description == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    result = event_description->createInstance(&one_shot_instance);
    if (result != FMOD_OK || one_shot_instance == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    FMOD_3D_ATTRIBUTES attributes = { { posX, posY, 0.0f }, { velX, velY, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } };
    result = one_shot_instance->set3DAttributes(&attributes);
    if (result != FMOD_OK)
    {
        one_shot_instance->release();
        ret->rvalue.val = 0.0;
        return;
    }

    result = one_shot_instance->start();
    if (result != FMOD_OK)
    {
        one_shot_instance->release();
        ret->rvalue.val = 0.0;
        return;
    }

    one_shot_instance->release();
    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_event_instance_is_playing(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        FMOD_STUDIO_PLAYBACK_STATE state;
        FMOD_RESULT result = fms_event_instance->getPlaybackState(&state);
        if (result == FMOD_OK && state == FMOD_STUDIO_PLAYBACK_PLAYING)
        {
            ret->rvalue.val = 1.0;
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_get_timeline_pos(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        int timeline_position = 0;
        FMOD_RESULT result = fms_event_instance->getTimelinePosition(&timeline_position);
        if (result == FMOD_OK)
        {
            ret->rvalue.val = static_cast<double>(timeline_position);
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_event_instance_set_timeline_pos(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    if (fms_event_instance)
    {
        int timeline_position = static_cast<int>(YYGetInt32(args, 1));
        FMOD_RESULT result = fms_event_instance->setTimelinePosition(timeline_position);
        if (result == FMOD_OK)
        {
            ret->rvalue.val = 1.0;
        }
        else
        {
            ret->rvalue.val = 0.0;
        }
    }
    else
    {
        ret->rvalue.val = 0.0;
    }
}

ABI_ATTR void fmod_bank_load_sample_data(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    char  *bank_name = YYGetString(args, 0);
    std::string fmod_banks = gmloader_config.fmod_bank_path;
    std::string bank_path = fmod_banks + bank_name;

    FMOD::Studio::Bank* bank;
    FMOD_RESULT result = fms_studioSystem->loadBankFile(bank_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    if (result != FMOD_OK)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    result = bank->loadSampleData();
    if (result != FMOD_OK)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    ret->rvalue.val = 1.0;
}

ABI_ATTR void fmod_event_get_length(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    const char* event_path = YYGetString(args, 0);
    FMOD::Studio::EventDescription* event_description = nullptr;

    FMOD_RESULT result = fms_studioSystem->getEvent(event_path, &event_description);
    if (result != FMOD_OK || event_description == nullptr)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    int length = 0;
    result = event_description->getLength(&length);
    if (result != FMOD_OK)
    {
        ret->rvalue.val = 0.0;
        return;
    }

    ret->rvalue.val = static_cast<double>(length);
}

void patch_fmod(so_module *mod)
{
    Function_Add("fmod_init", fmod_init, 1, 0);
    Function_Add("fmod_destroy", fmod_destroy, 0, 0);
    Function_Add("fmod_bank_load", fmod_bank_load, 1, 0);
    Function_Add("fmod_update", fmod_update, 0, 0);
    Function_Add("fmod_event_create_instance", fmod_event_create_instance, 1, 0);
    Function_Add("fmod_event_instance_play", fmod_event_instance_play, 0, 0);
    Function_Add("fmod_event_instance_stop", fmod_event_instance_stop, 0, 0);
    Function_Add("fmod_event_instance_release", fmod_event_instance_release, 0, 0);
    Function_Add("fmod_event_instance_set_3d_attributes", fmod_event_instance_set_3d_attributes, 3, 0);
    Function_Add("fmod_set_listener_attributes", fmod_set_listener_attributes, 3, 0);
    Function_Add("fmod_set_num_listeners", fmod_set_num_listeners, 1, 0);
    Function_Add("fmod_event_instance_set_parameter", fmod_event_instance_set_parameter, 3, 0);
    Function_Add("fmod_event_instance_get_parameter", fmod_event_instance_get_parameter, 1, 0);
    Function_Add("fmod_set_parameter", fmod_set_parameter, 2, 0);
    Function_Add("fmod_get_parameter", fmod_get_parameter, 1, 0);
    Function_Add("fmod_event_instance_set_paused", fmod_event_instance_set_paused, 2, 0);
    Function_Add("fmod_event_instance_get_paused", fmod_event_instance_get_paused, 0, 0);
    Function_Add("fmod_event_instance_set_paused_all", fmod_event_instance_set_paused_all, 1, 0);
    Function_Add("fmod_event_one_shot", fmod_event_one_shot, 1, 0);
    Function_Add("fmod_event_one_shot_3d", fmod_event_one_shot_3d, 3, 0);
    Function_Add("fmod_event_instance_is_playing", fmod_event_instance_is_playing, 0, 0);
    Function_Add("fmod_event_instance_get_timeline_pos", fmod_event_instance_get_timeline_pos, 0, 0);
    Function_Add("fmod_event_instance_set_timeline_pos", fmod_event_instance_set_timeline_pos, 2, 0);
    Function_Add("fmod_bank_load_sample_data", fmod_bank_load_sample_data, 1, 0);
    Function_Add("fmod_event_get_length", fmod_event_get_length, 1, 0);
}

#endif