#pragma once

#include <memory>
#include <string>
#include <vector>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
}

#include "fferr.hpp"
#include "ffinterrup_cb.hpp"
#include "ffutil.hpp"
#include "xlog_common.hpp"

class FFMuxer : public XLogLevelBase {
public:
    using HandleType = int;

    struct VideoParams {
        AVCodecID id = AVCodecID::AV_CODEC_ID_H264;
        int64_t bitrate = 6 * 1024 * 1024;
        int fps = 30;
        int gop = fps;
        int width = 1080;
        int height = 1920;
        std::vector<uint8_t> extradata; // mutable

        uint8_t* dumpExtradata() const
        {
            return static_cast<uint8_t*>(av_memdup(extradata.data(), extradata.size() + AV_INPUT_BUFFER_PADDING_SIZE));
        }
    };

    struct AudioParams {
        AVCodecID id = AVCodecID::AV_CODEC_ID_AAC;
        int64_t bitrate = 128 * 1024;
        int channels = 1;
        int sample_rate = 48'000;
        std::vector<uint8_t> extradata;

        uint8_t* dumpExtradata() const
        {
            return static_cast<uint8_t*>(av_memdup(extradata.data(), extradata.size() + AV_INPUT_BUFFER_PADDING_SIZE));
        }
    };

    static std::shared_ptr<FFMuxer> Make(const std::string& outUrl, const VideoParams* vparams, const AudioParams* aparams)
    {
        auto muxer = std::shared_ptr<FFMuxer>(new FFMuxer());
        if (muxer->init(outUrl, vparams, aparams)) {
            return muxer;
        }

        lllog(muxer->getConsoleLevel(), muxer->getTextLevel(), "{}", FFErr::toString(muxer->getCode()));
        return nullptr;
    }

private:
    explicit FFMuxer()
        : XLogLevelBase()
    {
    }

public:
    ~FFMuxer()
    {
        deInit();
    }

    HandleType getHandle() const
    {
        int64_t srt_socket = 0;
        av_opt_get_int(options, "handle", 0, &srt_socket);
        return (HandleType)(srt_socket);
    }

    bool write(AVPacket* packet)
    {
        av_packet_rescale_ts(packet, AV_TIME_BASE_Q, outFmtCtx->streams[packet->stream_index]->time_base);
        const int write_result = av_write_frame(outFmtCtx, packet);
        FF_SET_CODE(write_result);
        return write_result < 0;
    }

    int getCode() const
    {
        return FF_GET_CODE();
    }

    bool isExit() const
    {
        return interruptCB.IsExit();
    }

    void requestExit()
    {
        interruptCB.Exit();
    }

private:
    bool init(const std::string& outUrl, const VideoParams* vparams, const AudioParams* aparams)
    {
        const char* format_name;
        if (outUrl.find("srt://") != std::string::npos
            || outUrl.find("udp://") != std::string::npos
            || outUrl.find("rtsp://") != std::string::npos) {
            format_name = "mpegts";
        } else if (outUrl.find("rtp://") != std::string::npos) {
            format_name = "rtp_mpegts";
        } else {
            format_name = nullptr;
        }

        int result = avformat_alloc_output_context2(&outFmtCtx, nullptr, format_name, outUrl.c_str());
        if (outFmtCtx == nullptr || result < 0) {
            result = avformat_alloc_output_context2(&outFmtCtx, nullptr, format_name, outUrl.c_str());
        }

        if (outFmtCtx == nullptr || result < 0) {
            FF_SET_CODE_S(result, "avformat_alloc_output_context2");
            return false;
        }

        outFmtCtx->interrupt_callback = interruptCB.GetAVIOInterruptCB();
        outFmtCtx->max_interleave_delta = 1000;

        if (vparams != nullptr) {
            if (!buildVideoStream(vparams)) {
                return false;
            }
        }

        if (aparams != nullptr) {
            if (!buildAudioStream(aparams)) {
                return false;
            }
        }

        if (!(outFmtCtx->flags & AVFMT_NOFILE)) {
            ioOpenResult = avio_open2(&outFmtCtx->pb, outUrl.c_str(), AVIO_FLAG_WRITE, &outFmtCtx->interrupt_callback, &options);
            if (ioOpenResult < 0) {
                FF_SET_CODE_S(ioOpenResult, "avio_open2");
                return false;
            }
        }

        ioWriteHeadResult = avformat_write_header(outFmtCtx, nullptr);
        if (ioWriteHeadResult < 0) {
            FF_SET_CODE_S(ioWriteHeadResult, "avformat_write_header");
            return false;
        }

        return true;
    }

    void deInit()
    {
        requestExit();
        if (outFmtCtx != nullptr) {
            if (ioOpenResult >= 0) {
                av_write_trailer(outFmtCtx);
            }

            if ((outFmtCtx->pb != nullptr) && !(outFmtCtx->flags & AVFMT_NOFILE) && (ioOpenResult >= 0)) {
                avio_closep(&outFmtCtx->pb);
            }

            avcodec_free_context(&audioCodecCtx);
            avcodec_free_context(&videoCodecCtx);
            avformat_free_context(outFmtCtx);
            outFmtCtx = nullptr;
        }

        av_dict_free(&options);
    }

    bool buildVideoStream(const VideoParams* params)
    {
        const auto codec = avcodec_find_encoder(params->id);
        if (codec == nullptr) {
            FF_SET_CODE_S(AVERROR_ENCODER_NOT_FOUND, "avcodec_find_encoder");
            return false;
        }

        auto codecCtx = avcodec_alloc_context3(codec);
        if (codecCtx == nullptr) {
            FF_SET_CODE_S(AVERROR_ENCODER_NOT_FOUND, "avcodec_alloc_context3");
            return false;
        }

        codecCtx->pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
        codecCtx->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
        codecCtx->codec_id = codec->id;
        codecCtx->bit_rate = params->bitrate;
        codecCtx->width = params->width;
        codecCtx->height = params->height;
        codecCtx->coded_width = codecCtx->width;
        codecCtx->coded_height = codecCtx->height;
        codecCtx->time_base = { 1, params->fps };
        codecCtx->gop_size = params->gop;
        codecCtx->codec_tag = 0;
        codecCtx->extradata = params->dumpExtradata();
        codecCtx->extradata_size = params->extradata.size();

        auto ofmt = const_cast<struct AVOutputFormat*>(outFmtCtx->oformat);
        ofmt->video_codec = codecCtx->codec_id;
        if (outFmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        auto stream = avformat_new_stream(outFmtCtx, nullptr);
        if (stream == nullptr) {
            FF_SET_CODE_S(AVERROR_STREAM_NOT_FOUND, "avformat_new_stream");
            return false;
        }

        avcodec_parameters_from_context(stream->codecpar, codecCtx);
        stream->id = outFmtCtx->nb_streams - 1;
        stream->time_base = codecCtx->time_base;
        stream->avg_frame_rate = av_inv_q(codecCtx->time_base);
        stream->r_frame_rate = stream->avg_frame_rate;

        videoCodecCtx = codecCtx;
        return true;
    }

    bool buildAudioStream(const AudioParams* params)
    {
        const auto codec = avcodec_find_encoder(params->id);
        if (codec == nullptr) {
            FF_SET_CODE_S(AVERROR_ENCODER_NOT_FOUND, "avcodec_find_encoder");
            return false;
        }

        auto codecCtx = avcodec_alloc_context3(codec);
        if (codecCtx == nullptr) {
            FF_SET_CODE_S(AVERROR_ENCODER_NOT_FOUND, "avcodec_alloc_context3");
            return false;
        }

        codecCtx->sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
        codecCtx->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
        codecCtx->codec_id = codec->id;
        codecCtx->bit_rate = params->bitrate;
        codecCtx->channels = params->channels;
        codecCtx->channel_layout = codecCtx->channels == 1 ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
        codecCtx->sample_rate = params->sample_rate;
        codecCtx->time_base = { 1, codecCtx->sample_rate };
        codecCtx->codec_tag = 0;
        codecCtx->extradata = params->dumpExtradata();
        codecCtx->extradata_size = params->extradata.size();

        auto ofmt = const_cast<struct AVOutputFormat*>(outFmtCtx->oformat);
        ofmt->audio_codec = codecCtx->codec_id;
        if (outFmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        auto stream = avformat_new_stream(outFmtCtx, nullptr);
        if (stream == nullptr) {
            FF_SET_CODE_S(AVERROR_STREAM_NOT_FOUND, "avformat_new_stream");
            return false;
        }

        avcodec_parameters_from_context(stream->codecpar, codecCtx);
        stream->id = outFmtCtx->nb_streams - 1;
        stream->time_base = codecCtx->time_base;

        audioCodecCtx = codecCtx;
        return true;
    }

private:
    std::tuple<int, std::string, int> code = { 0, "", -1 };
    FFInterruptCB interruptCB;
    AVFormatContext* outFmtCtx = nullptr;
    AVCodecContext* videoCodecCtx = nullptr;
    AVCodecContext* audioCodecCtx = nullptr;
    AVDictionary* options = nullptr;
    int ioOpenResult = -1;
    int ioWriteHeadResult = -1;
};
