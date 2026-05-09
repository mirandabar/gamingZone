#include "../include/VideoRecorder.h"
#include "../include/Logger.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

static const std::string FILE_NAME = "VideoRecorder.cpp";

VideoRecorder::VideoRecorder(const std::string& filePath, int width, int height, int fps)
    : m_filePath(filePath), m_width(width), m_height(height), m_fps(fps),
      m_isInitialized(false), m_formatContext(nullptr), m_codecContext(nullptr),
      m_stream(nullptr), m_frame(nullptr), m_swsContext(nullptr), m_frameCount(0) {
    Logger::debug(FILE_NAME, "VideoRecorder::VideoRecorder", 
                  "Creating VideoRecorder: " + filePath + " (" + std::to_string(width) + 
                  "x" + std::to_string(height) + " @ " + std::to_string(fps) + " fps)");
}

VideoRecorder::~VideoRecorder() {
    Logger::debug(FILE_NAME, "VideoRecorder::~VideoRecorder", "Destroying VideoRecorder");
    finalize();
}

bool VideoRecorder::initialize() {
    Logger::info(FILE_NAME, "VideoRecorder::initialize", "Initializing video recording");
    
    if (m_isInitialized) {
        Logger::warning(FILE_NAME, "VideoRecorder::initialize", "VideoRecorder already initialized");
        return true;
    }

    // Allocate format context
    avformat_alloc_output_context2(&m_formatContext, nullptr, nullptr, m_filePath.c_str());
    if (!m_formatContext) {
        Logger::critical(FILE_NAME, "VideoRecorder::initialize", 
                        "Could not allocate format context for: " + m_filePath);
        return false;
    }

    if (!setupCodecAndContext()) {
        Logger::critical(FILE_NAME, "VideoRecorder::initialize", "Failed to setup codec and context");
        return false;
    }

    // Open output file
    if (!(m_formatContext->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&m_formatContext->pb, m_filePath.c_str(), AVIO_FLAG_WRITE) < 0) {
            Logger::critical(FILE_NAME, "VideoRecorder::initialize", 
                            "Could not open output file: " + m_filePath);
            return false;
        }
    }

    // Write header
    if (avformat_write_header(m_formatContext, nullptr) < 0) {
        Logger::critical(FILE_NAME, "VideoRecorder::initialize", "Error writing header");
        return false;
    }

    m_isInitialized = true;
    Logger::info(FILE_NAME, "VideoRecorder::initialize", "Video recording initialized successfully");
    return true;
}

bool VideoRecorder::setupCodecAndContext() {
    Logger::debug(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Setting up codec and context");
    
    // Find encoder
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "H264 codec not found");
        return false;
    }

    // Create stream
    m_stream = avformat_new_stream(m_formatContext, codec);
    if (!m_stream) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Could not create stream");
        return false;
    }

    // Create codec context
    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Could not allocate codec context");
        return false;
    }

    // Set parameters
    m_codecContext->codec_id = AV_CODEC_ID_H264;
    m_codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    m_codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    m_codecContext->width = m_width;
    m_codecContext->height = m_height;
    m_codecContext->time_base = {1, m_fps};
    m_codecContext->framerate = {m_fps, 1};
    m_codecContext->bit_rate = 12000000; // 12Mbps for social media quality
    m_codecContext->gop_size = 10; // Keyframe interval
    m_codecContext->max_b_frames = 3;

    // Copy parameters to stream
    avcodec_parameters_from_context(m_stream->codecpar, m_codecContext);

    // Set H.264 encoder options for better quality
    AVDictionary* options = nullptr;
    av_dict_set(&options, "preset", "medium", 0);     // medium quality preset
    av_dict_set(&options, "crf", "18", 0);            // Higher quality (lower CRF = better, 0-51)
    av_dict_set(&options, "profile", "main", 0);      // Main profile for compatibility
    av_dict_set(&options, "rc", "cbr", 0);            // Constant bitrate mode
    av_dict_set(&options, "minrate", "12000k", 0);    // Minimum bitrate 12 Mbps
    av_dict_set(&options, "maxrate", "12000k", 0);    // Maximum bitrate 12 Mbps
    av_dict_set(&options, "bufsize", "24000k", 0);    // Buffer size (2x max bitrate)
    av_dict_set(&options, "rc-lookahead", "40", 0);   // Better rate control lookahead

    // Open codec
    if (avcodec_open2(m_codecContext, codec, &options) < 0) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Could not open codec");
        av_dict_free(&options);
        return false;
    }
    av_dict_free(&options);

    // Allocate frame
    m_frame = av_frame_alloc();
    if (!m_frame) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Could not allocate frame");
        return false;
    }

    m_frame->format = m_codecContext->pix_fmt;
    m_frame->width = m_codecContext->width;
    m_frame->height = m_codecContext->height;

    if (av_frame_get_buffer(m_frame, 0) < 0) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Could not allocate frame buffer");
        return false;
    }

    // Create software scaler (to convert RGBA to YUV420p with high quality)
    m_swsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_RGBA,
                                  m_width, m_height, AV_PIX_FMT_YUV420P,
                                  SWS_GAUSS, nullptr, nullptr, nullptr);  // SWS_GAUSS for better quality
    if (!m_swsContext) {
        Logger::critical(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Could not create scaler context");
        return false;
    }

    Logger::debug(FILE_NAME, "VideoRecorder::setupCodecAndContext", "Codec and context setup successful");
    return true;
}

bool VideoRecorder::captureFrame(const uint8_t* pixelData, int dataSize) {
    if (!m_isInitialized) {
        Logger::warning(FILE_NAME, "VideoRecorder::captureFrame", "VideoRecorder not initialized");
        return false;
    }

    if (!pixelData || dataSize < m_width * m_height * 4) {
        Logger::warning(FILE_NAME, "VideoRecorder::captureFrame", "Invalid pixel data");
        return false;
    }

    // Convert RGB32 to YUV420p
    const uint8_t* srcData[1] = {pixelData};
    int srcLinesize[1] = {m_width * 4};

    if (sws_scale(m_swsContext, srcData, srcLinesize, 0, m_height,
                  m_frame->data, m_frame->linesize) < 0) {
        Logger::error(FILE_NAME, "VideoRecorder::captureFrame", "Error during frame conversion");
        return false;
    }

    // Set frame timestamp
    m_frame->pts = m_frameCount++;

    if (!writeFrame(m_frame)) {
        Logger::error(FILE_NAME, "VideoRecorder::captureFrame", "Failed to write frame");
        return false;
    }

    return true;
}

bool VideoRecorder::writeFrame(AVFrame* frame) {
    int ret = avcodec_send_frame(m_codecContext, frame);
    if (ret < 0) {
        Logger::error(FILE_NAME, "VideoRecorder::writeFrame", "Error sending frame to encoder");
        return false;
    }

    AVPacket* packet = av_packet_alloc();
    if (!packet) {
        Logger::error(FILE_NAME, "VideoRecorder::writeFrame", "Could not allocate packet");
        return false;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(m_codecContext, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            Logger::error(FILE_NAME, "VideoRecorder::writeFrame", "Error receiving packet from encoder");
            av_packet_free(&packet);
            return false;
        }

        packet->stream_index = m_stream->index;
        av_packet_rescale_ts(packet, m_codecContext->time_base, m_stream->time_base);

        if (av_interleaved_write_frame(m_formatContext, packet) < 0) {
            Logger::error(FILE_NAME, "VideoRecorder::writeFrame", "Error writing packet");
            av_packet_free(&packet);
            return false;
        }
    }

    av_packet_free(&packet);
    return true;
}

bool VideoRecorder::finalize() {
    if (!m_isInitialized) {
        return true;
    }

    Logger::info(FILE_NAME, "VideoRecorder::finalize", "Finalizing video recording");

    // Flush encoder
    if (m_codecContext) {
        avcodec_send_frame(m_codecContext, nullptr);
        AVPacket* packet = av_packet_alloc();
        if (packet) {
            while (avcodec_receive_packet(m_codecContext, packet) >= 0) {
                packet->stream_index = m_stream->index;
                av_packet_rescale_ts(packet, m_codecContext->time_base, m_stream->time_base);
                av_interleaved_write_frame(m_formatContext, packet);
            }
            av_packet_free(&packet);
        }
    }

    // Write trailer
    if (m_formatContext) {
        av_write_trailer(m_formatContext);
    }

    // Clean up resources
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_codecContext) {
        avcodec_free_context(&m_codecContext);
    }
    if (m_formatContext) {
        if (!(m_formatContext->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&m_formatContext->pb);
        }
        avformat_free_context(m_formatContext);
    }
    if (m_swsContext) {
        sws_freeContext(m_swsContext);
    }

    m_isInitialized = false;
    Logger::info(FILE_NAME, "VideoRecorder::finalize", "Video recording finalized. Saved to: " + m_filePath);
    return true;
}
