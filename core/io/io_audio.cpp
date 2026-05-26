#include "io_audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_engine audio_engine;
Fixed_Handle_Array<ma_sound> sounds;
bool audio_inited = false;

fn io_audio_init() -> bool {

    if (audio_inited) {
        io_audio_done();
    }

    logf("Audio System created!");
    auto result = ma_engine_init(nullptr, &audio_engine);
    if (result != MA_SUCCESS) 
    {
        checkf(false, "Error! Failed to initialize the audio engine!");
        return false;
    }
    audio_inited = true;
    return true;
}

fn io_audio_done() -> void {
    if (audio_inited) {
        logf("Audio System destroyed!");
        ma_engine_uninit(&audio_engine);
        audio_inited = false;
    }
    reset(&sounds);
}

fn io_audio_load(const std::string& filename) -> Audio_Handle {
    auto handle = append(&sounds);
    auto* sound = get(sounds, handle);
    auto result = ma_sound_init_from_file(&audio_engine, filename.c_str(), 0, nullptr, nullptr, sound);
    if (result != MA_SUCCESS)
    {
        checkf(false, "Error! Failed to load the sound!");
        return {};
    }
    return handle;
}

fn io_audio_free(const Audio_Handle& handle) -> void {
    auto* sound = get(sounds, handle);
    if (sound) {
        ma_sound_uninit(sound);
    }
}

fn io_audio_play(const Audio_Handle& handle) -> void {
    auto* sound = get(sounds, handle);
    if (sound) {
        ma_sound_start(sound);
    }
}

fn io_audio_stop(const Audio_Handle& handle) -> void {
    auto* sound = get(sounds, handle);
    if (sound) {
        ma_sound_stop(sound);
    }
}

fn io_audio_set_volume(const Audio_Handle& handle, f32 volume) -> void {
    auto* sound = get(sounds, handle);
    if (sound) {
        ma_sound_set_volume(sound, volume);
    }
}