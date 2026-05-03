#include "../include/Audio.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <iostream>

bool Audio::initialized = false;

bool Audio::initialize() {
    if (initialized) return true;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "[Audio] Error: SDL_Init failed - " << SDL_GetError() << std::endl;
        return false;
    }

    int flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;
    if (Mix_Init(flags) == 0) {
        std::cerr << "[Audio] Warning: Mix_Init failed - " << Mix_GetError() << std::endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        std::cerr << "[Audio] Error: Mix_OpenAudio failed - " << Mix_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    Mix_AllocateChannels(32);
    initialized = true;
    std::cout << "[Audio] Sistema de audio inicializado exitosamente" << std::endl;
    return true;
}

void Audio::shutdown() {
    if (!initialized) return;

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    initialized = false;
    std::cout << "[Audio] Sistema de audio cerrado" << std::endl;
}

bool Audio::playSound(const std::string& filepath, int loops) {
    if (!initialized) {
        std::cerr << "[Audio] Error: Sistema de audio no inicializado" << std::endl;
        return false;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
        std::cerr << "[Audio] Error al cargar sonido: " << filepath << " - " << Mix_GetError() << std::endl;
        return false;
    }

    if (Mix_PlayChannel(-1, chunk, loops) == -1) {
        std::cerr << "[Audio] Error al reproducir sonido: " << Mix_GetError() << std::endl;
        Mix_FreeChunk(chunk);
        return false;
    }

    // Nota: SDL_mixer libera automaticamente los chunks, así que no llamamos Mix_FreeChunk aquí
    return true;
}

bool Audio::playMusic(const std::string& filepath, int loops) {
    if (!initialized) {
        std::cerr << "[Audio] Error: Sistema de audio no inicializado" << std::endl;
        return false;
    }

    Mix_Music* music = Mix_LoadMUS(filepath.c_str());
    if (!music) {
        std::cerr << "[Audio] Error al cargar música: " << filepath << " - " << Mix_GetError() << std::endl;
        return false;
    }

    if (Mix_PlayMusic(music, loops) == -1) {
        std::cerr << "[Audio] Error al reproducir música: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(music);
        return false;
    }

    return true;
}

void Audio::stopMusic() {
    if (initialized) {
        Mix_HaltMusic();
    }
}

void Audio::pauseMusic() {
    if (initialized && Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void Audio::resumeMusic() {
    if (initialized && Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void Audio::setVolume(int volume) {
    if (!initialized) return;

    // Limita el volumen entre 0 y 128
    volume = (volume < 0) ? 0 : (volume > 128) ? 128 : volume;
    Mix_Volume(-1, volume);
}

int Audio::getVolume() {
    if (!initialized) return 0;
    return Mix_Volume(-1, -1);
}

void Audio::beep(int frequency, int duration, int volume) {
    if (!initialized) {
        std::cerr << "[Audio] Error: Sistema de audio no inicializado" << std::endl;
        return;
    }

    // Limita parámetros
    frequency = (frequency < 20) ? 20 : (frequency > 20000) ? 20000 : frequency;
    duration = (duration < 10) ? 10 : (duration > 5000) ? 5000 : duration;
    volume = (volume < 0) ? 0 : (volume > 128) ? 128 : volume;

    // Parámetros de audio
    const int sample_rate = 44100;
    const int num_samples = (sample_rate * duration) / 1000;
    const SDL_AudioFormat format = AUDIO_F32;

    // Crea un buffer de audio con forma de onda sinusoidal
    float* samples = new float[num_samples];
    for (int i = 0; i < num_samples; ++i) {
        // Genera onda sinusoidal amortiguada (fade out)
        float t = static_cast<float>(i) / sample_rate;
        float envelope = std::exp(-3.0f * t / (duration / 1000.0f));  // Decaimiento exponencial
        samples[i] = envelope * std::sin(2.0f * M_PI * frequency * t) * (volume / 128.0f);
    }

    // Crea un chunk de SDL_mixer
    SDL_AudioSpec desired, obtained;
    SDL_zero(desired);
    desired.freq = sample_rate;
    desired.format = format;
    desired.channels = 1;
    desired.samples = 4096;
    desired.callback = nullptr;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (device == 0) {
        std::cerr << "[Audio] Error al abrir dispositivo de audio para beep: " << SDL_GetError() << std::endl;
        delete[] samples;
        return;
    }

    // Convierte float a el formato necesario
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(reinterpret_cast<Uint8*>(samples), num_samples * sizeof(float));
    if (!chunk) {
        std::cerr << "[Audio] Error al crear chunk de audio: " << Mix_GetError() << std::endl;
        SDL_CloseAudioDevice(device);
        delete[] samples;
        return;
    }

    if (Mix_PlayChannel(-1, chunk, 0) == -1) {
        std::cerr << "[Audio] Error al reproducir beep: " << Mix_GetError() << std::endl;
    }

    SDL_CloseAudioDevice(device);
    delete[] samples;
}
