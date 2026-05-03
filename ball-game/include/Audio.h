#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <memory>

class Audio {
public:
    /**
     * @brief Inicializa el sistema de audio
     * @return true si la inicialización fue exitosa, false en caso contrario
     */
    static bool initialize();

    /**
     * @brief Libera los recursos del sistema de audio
     */
    static void shutdown();

    /**
     * @brief Reproduce un archivo de sonido
     * @param filepath Ruta del archivo de audio (.wav, .ogg, .flac, .mp3)
     * @param loops Número de repeticiones (-1 para infinito, 0 para una sola vez)
     * @return true si el sonido se reprodujo exitosamente
     */
    static bool playSound(const std::string& filepath, int loops = 0);

    /**
     * @brief Reproduce música de fondo
     * @param filepath Ruta del archivo de música
     * @param loops Número de repeticiones (-1 para infinito)
     * @return true si la música se reprodujo exitosamente
     */
    static bool playMusic(const std::string& filepath, int loops = -1);

    /**
     * @brief Detiene la música que se está reproduciendo
     */
    static void stopMusic();

    /**
     * @brief Pausa la música actual
     */
    static void pauseMusic();

    /**
     * @brief Reanuda la música pausada
     */
    static void resumeMusic();

    /**
     * @brief Establece el volumen general (0-128)
     * @param volume Nivel de volumen
     */
    static void setVolume(int volume);

    /**
     * @brief Obtiene el volumen actual (0-128)
     * @return Nivel de volumen
     */
    static int getVolume();

    /**
     * @brief Genera y reproduce un tono simple (beep)
     * @param frequency Frecuencia en Hz (ej: 440 para La)
     * @param duration Duración en milisegundos
     * @param volume Volumen (0-128)
     */
    static void beep(int frequency = 440, int duration = 100, int volume = 64);

private:
    Audio() = delete;  // No se puede instanciar
    ~Audio() = delete;

    static bool initialized;
};

#endif // AUDIO_H
