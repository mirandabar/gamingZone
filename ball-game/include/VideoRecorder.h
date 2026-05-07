#pragma once

#include <string>
#include <cstdint>
#include <memory>

// FFmpeg forward declarations
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct SwsContext;

class VideoRecorder {
public:
    VideoRecorder(const std::string& filePath, int width, int height, int fps = 60);
    ~VideoRecorder();
    
    // Rule of Five - forbid copying system resources, but allow move
    VideoRecorder(const VideoRecorder&) = delete;
    VideoRecorder& operator=(const VideoRecorder&) = delete;
    VideoRecorder(VideoRecorder&&) = default;
    VideoRecorder& operator=(VideoRecorder&&) = default;

    bool initialize();
    bool captureFrame(const uint8_t* pixelData, int dataSize);
    bool finalize();

    bool isInitialized() const { return m_isInitialized; }

private:
    std::string m_filePath;
    int m_width;
    int m_height;
    int m_fps;
    bool m_isInitialized;
    
    AVFormatContext* m_formatContext;
    AVCodecContext* m_codecContext;
    AVStream* m_stream;
    AVFrame* m_frame;
    SwsContext* m_swsContext;
    int64_t m_frameCount;

    bool setupCodecAndContext();
    bool writeFrame(AVFrame* frame);
};
