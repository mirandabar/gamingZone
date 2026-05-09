#include "../include/VideoRecorder.h"
#include "../include/Logger.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <filesystem>

static const std::string FILE_NAME = "VideoRecorder.cpp";

static std::string ffmpegErrorToString(int errorCode) {
	char buffer[AV_ERROR_MAX_STRING_SIZE] = {0};
	av_strerror(errorCode, buffer, sizeof(buffer));
	return std::string(buffer);
}

VideoRecorder::VideoRecorder(const std::string& filename, int width, int height, int fps)
	: m_filename(filename),
	  m_width(width),
	  m_height(height),
	  m_fps(fps),
	  m_initialized(false),
	  m_frameIndex(0),
	  m_formatCtx(nullptr),
	  m_codecCtx(nullptr),
	  m_stream(nullptr),
	  m_frame(nullptr),
	  m_packet(nullptr),
	  m_swsCtx(nullptr) {
	Logger::debug(FILE_NAME, "VideoRecorder::VideoRecorder", "VideoRecorder created");
}

VideoRecorder::~VideoRecorder() {
	finalize();
}

bool VideoRecorder::initialize() {
	if (m_initialized) {
		return true;
	}

	if (m_filename.empty() || m_width <= 0 || m_height <= 0 || m_fps <= 0) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Invalid video configuration");
		return false;
	}

	if ((m_width % 2) != 0 || (m_height % 2) != 0) {
		Logger::warning(FILE_NAME, "VideoRecorder::initialize", "Video dimensions are not even; encoder may fail");
	}

	std::filesystem::path outputPath(m_filename);
	std::error_code dirError;
	if (!outputPath.parent_path().empty()) {
		std::filesystem::create_directories(outputPath.parent_path(), dirError);
		if (dirError) {
			Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to create output directory: " + dirError.message());
			return false;
		}
	}

	int ret = avformat_alloc_output_context2(&m_formatCtx, nullptr, nullptr, m_filename.c_str());
	if (ret < 0 || !m_formatCtx) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to allocate format context: " + ffmpegErrorToString(ret));
		cleanup();
		return false;
	}

	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec) {
		Logger::warning(FILE_NAME, "VideoRecorder::initialize", "H264 encoder not found, trying MPEG4");
		codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
	}
	if (!codec) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "No suitable video encoder found");
		cleanup();
		return false;
	}

	m_stream = avformat_new_stream(m_formatCtx, nullptr);
	if (!m_stream) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to create video stream");
		cleanup();
		return false;
	}

	m_codecCtx = avcodec_alloc_context3(codec);
	if (!m_codecCtx) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to allocate codec context");
		cleanup();
		return false;
	}

	m_codecCtx->codec_id = codec->id;
	m_codecCtx->width = m_width;
	m_codecCtx->height = m_height;
	m_codecCtx->time_base = AVRational{1, m_fps};
	m_codecCtx->framerate = AVRational{m_fps, 1};
	m_codecCtx->gop_size = 10;
	m_codecCtx->max_b_frames = 3;
	m_codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	m_codecCtx->bit_rate = 12000000;

	if (m_formatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
		m_codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	AVDictionary* codecOptions = nullptr;
	if (codec->id == AV_CODEC_ID_H264) {
		av_dict_set(&codecOptions, "preset", "medium", 0);
		av_dict_set(&codecOptions, "crf", "22", 0);
		av_dict_set(&codecOptions, "profile", "main", 0);
	}

	ret = avcodec_open2(m_codecCtx, codec, &codecOptions);
	av_dict_free(&codecOptions);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to open codec: " + ffmpegErrorToString(ret));
		cleanup();
		return false;
	}

	ret = avcodec_parameters_from_context(m_stream->codecpar, m_codecCtx);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to copy codec parameters: " + ffmpegErrorToString(ret));
		cleanup();
		return false;
	}
	m_stream->time_base = m_codecCtx->time_base;

	if (!(m_formatCtx->oformat->flags & AVFMT_NOFILE)) {
		ret = avio_open(&m_formatCtx->pb, m_filename.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to open output file: " + ffmpegErrorToString(ret));
			cleanup();
			return false;
		}
	}

	ret = avformat_write_header(m_formatCtx, nullptr);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to write header: " + ffmpegErrorToString(ret));
		cleanup();
		return false;
	}

	m_frame = av_frame_alloc();
	if (!m_frame) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to allocate frame");
		cleanup();
		return false;
	}
	m_frame->format = m_codecCtx->pix_fmt;
	m_frame->width = m_codecCtx->width;
	m_frame->height = m_codecCtx->height;

	ret = av_frame_get_buffer(m_frame, 32);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to allocate frame buffer: " + ffmpegErrorToString(ret));
		cleanup();
		return false;
	}

	m_packet = av_packet_alloc();
	if (!m_packet) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to allocate packet");
		cleanup();
		return false;
	}

	m_swsCtx = sws_getContext(m_width, m_height, AV_PIX_FMT_RGB24,
							  m_width, m_height, AV_PIX_FMT_YUV420P,
							  SWS_GAUSS, nullptr, nullptr, nullptr);
	if (!m_swsCtx) {
		Logger::error(FILE_NAME, "VideoRecorder::initialize", "Failed to initialize color converter");
		cleanup();
		return false;
	}

	m_frameIndex = 0;
	m_initialized = true;
	Logger::info(FILE_NAME, "VideoRecorder::initialize", "Video recording initialized");
	return true;
}

void VideoRecorder::captureFrame(const uint8_t* data, int size) {
	if (!m_initialized) {
		return;
	}

	if (!data || size <= 0) {
		Logger::warning(FILE_NAME, "VideoRecorder::captureFrame", "Empty frame received");
		return;
	}

	if (!encodeFrame(data, size)) {
		Logger::warning(FILE_NAME, "VideoRecorder::captureFrame", "Failed to encode frame");
	}
}

bool VideoRecorder::encodeFrame(const uint8_t* data, int size) {
	constexpr int kBytesPerPixel = 3; // RGB24
	int stride = size / m_height;
	if (stride <= 0 || (stride * m_height) != size) {
		Logger::error(FILE_NAME, "VideoRecorder::encodeFrame", "Invalid frame buffer size");
		return false;
	}

	if (stride < m_width * kBytesPerPixel) {
		Logger::error(FILE_NAME, "VideoRecorder::encodeFrame", "Frame stride is smaller than expected");
		return false;
	}

	int ret = av_frame_make_writable(m_frame);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::encodeFrame", "Frame not writable: " + ffmpegErrorToString(ret));
		return false;
	}

	const uint8_t* srcSlices[1] = {data};
	int srcStride[1] = {stride};

	sws_scale(m_swsCtx, srcSlices, srcStride, 0, m_height, m_frame->data, m_frame->linesize);

	m_frame->pts = m_frameIndex++;

	ret = avcodec_send_frame(m_codecCtx, m_frame);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::encodeFrame", "Failed to send frame: " + ffmpegErrorToString(ret));
		return false;
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(m_codecCtx, m_packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			break;
		}
		if (ret < 0) {
			Logger::error(FILE_NAME, "VideoRecorder::encodeFrame", "Failed to receive packet: " + ffmpegErrorToString(ret));
			return false;
		}

		av_packet_rescale_ts(m_packet, m_codecCtx->time_base, m_stream->time_base);
		m_packet->stream_index = m_stream->index;

		ret = av_interleaved_write_frame(m_formatCtx, m_packet);
		av_packet_unref(m_packet);
		if (ret < 0) {
			Logger::error(FILE_NAME, "VideoRecorder::encodeFrame", "Failed to write packet: " + ffmpegErrorToString(ret));
			return false;
		}
	}

	return true;
}

void VideoRecorder::finalize() {
	if (!m_initialized) {
		cleanup();
		return;
	}

	int ret = avcodec_send_frame(m_codecCtx, nullptr);
	if (ret >= 0) {
		while (true) {
			ret = avcodec_receive_packet(m_codecCtx, m_packet);
			if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
				break;
			}
			if (ret < 0) {
				Logger::error(FILE_NAME, "VideoRecorder::finalize", "Failed to flush packet: " + ffmpegErrorToString(ret));
				break;
			}

			av_packet_rescale_ts(m_packet, m_codecCtx->time_base, m_stream->time_base);
			m_packet->stream_index = m_stream->index;

			ret = av_interleaved_write_frame(m_formatCtx, m_packet);
			av_packet_unref(m_packet);
			if (ret < 0) {
				Logger::error(FILE_NAME, "VideoRecorder::finalize", "Failed to write flushed packet: " + ffmpegErrorToString(ret));
				break;
			}
		}
	} else if (ret != AVERROR_EOF) {
		Logger::error(FILE_NAME, "VideoRecorder::finalize", "Failed to flush encoder: " + ffmpegErrorToString(ret));
	}

	ret = av_write_trailer(m_formatCtx);
	if (ret < 0) {
		Logger::error(FILE_NAME, "VideoRecorder::finalize", "Failed to write trailer: " + ffmpegErrorToString(ret));
	}

	Logger::info(FILE_NAME, "VideoRecorder::finalize", "Video recording finalized");
	cleanup();
}

void VideoRecorder::cleanup() {
	if (m_packet) {
		av_packet_free(&m_packet);
	}

	if (m_frame) {
		av_frame_free(&m_frame);
	}

	if (m_swsCtx) {
		sws_freeContext(m_swsCtx);
		m_swsCtx = nullptr;
	}

	if (m_codecCtx) {
		avcodec_free_context(&m_codecCtx);
	}

	if (m_formatCtx) {
		if (!(m_formatCtx->oformat->flags & AVFMT_NOFILE) && m_formatCtx->pb) {
			avio_closep(&m_formatCtx->pb);
		}
		avformat_free_context(m_formatCtx);
		m_formatCtx = nullptr;
	}

	m_stream = nullptr;
	m_initialized = false;
}
