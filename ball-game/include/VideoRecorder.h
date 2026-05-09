#pragma once

#include <cstdint>
#include <string>

extern "C" {
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct AVPacket;
struct SwsContext;
}

class VideoRecorder {
public:
	VideoRecorder(const std::string& filename, int width, int height, int fps);
	~VideoRecorder();

	VideoRecorder(const VideoRecorder&) = delete;
	VideoRecorder& operator=(const VideoRecorder&) = delete;
	VideoRecorder(VideoRecorder&&) = delete;
	VideoRecorder& operator=(VideoRecorder&&) = delete;

	bool initialize();
	void captureFrame(const uint8_t* data, int size);
	void finalize();
	bool isInitialized() const { return m_initialized; }

private:
	bool encodeFrame(const uint8_t* data, int size);
	void cleanup();

	std::string m_filename;
	int m_width;
	int m_height;
	int m_fps;
	bool m_initialized;
	int64_t m_frameIndex;
	AVFormatContext* m_formatCtx;
	AVCodecContext* m_codecCtx;
	AVStream* m_stream;
	AVFrame* m_frame;
	AVPacket* m_packet;
	SwsContext* m_swsCtx;
};

