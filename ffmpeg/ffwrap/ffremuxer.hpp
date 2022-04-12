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

class FFRemuxer : public XLogLevelBase {
public:
    using HandleType = int;

private:
    explicit FFRemuxer()
        : XLogLevelBase()
    {
    }

public:
    static std::shared_ptr<FFRemuxer> Make(const std::string& inUrl, const std::string& outUrl)
    {
        auto muxer = std::shared_ptr<FFRemuxer>(new FFRemuxer());
        if (muxer->init(inUrl, outUrl)) {
            return muxer;
        }

        lllog(muxer->getConsoleLevel(), muxer->getTextLevel(), "{}", FFErr::toString(muxer->getCode()));
        return nullptr;
    }

    ~FFRemuxer()
    {
        deInit();
    }

    HandleType getHandle() const
    {
        int64_t srt_socket = 0;
        av_opt_get_int(options, "handle", 0, &srt_socket);
        return (HandleType)(srt_socket);
    }

    bool read(AVPacket* packet,const AVRational *timebase = nullptr)
    {
        const int read_result = av_read_frame(inFmtCtx, packet);
        FF_SET_CODE_S(read_result, "av_read_frame");
        if (read_result < 0) {
            return false;
        }

        if (packet->stream_index >= streamMapping.size() || streamMapping[packet->stream_index] < 0) {
            av_packet_unref(packet);
            FF_SET_CODE_S(read_result, "stream_index >= nb_stream");
            return false;
        }

        packet->stream_index = streamMapping[packet->stream_index];
        if(timebase != nullptr){
            av_packet_rescale_ts(packet, inFmtCtx->streams[packet->stream_index]->time_base, *timebase);
        }

        return read_result < 0;
    }

    bool write(AVPacket* packet,const AVRational *timebase = nullptr)
    {
        if(timebase != nullptr){
            av_packet_rescale_ts(packet, *timebase, outFmtCtx->streams[packet->stream_index]->time_base);
        }

        const int write_result = av_interleaved_write_frame(outFmtCtx, packet);
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
    bool init(const std::string& inUrl, const std::string& outUrl)
    {
        if (!initInFormatCtx(inUrl)) {
            deInit();
            return false;
        }

        if (!initOutFormatCtx(outUrl)) {
            deInit();
            return false;
        }

        return true;
    }

    void deInit()
    {
        requestExit();
        deInitOutFormatCtx();
        deInitInFormatCtx();
        av_dict_free(&options);
    }

    bool initInFormatCtx(const std::string& inUrl)
    {
        inFmtCtx = avformat_alloc_context();
        if (inFmtCtx == nullptr) {
            FF_SET_CODE_S(AVERROR_UNKNOWN, "avformat_alloc_context");
            return false;
        }

        inFmtCtx->interrupt_callback = interruptCB.GetAVIOInterruptCB();
        int result = avformat_open_input(&inFmtCtx, inUrl.c_str(), nullptr, &options);
        if (result < 0) {
            FF_SET_CODE_S(result, "avformat_open_input");
            return false;
        }

        if (inFmtCtx == nullptr) {
            FF_SET_CODE_S(AVERROR_UNKNOWN, "avformat_open_input");
            return false;
        }

        result = avformat_find_stream_info(inFmtCtx, &options);
        if (result < 0) {
            FF_SET_CODE_S(result, "avformat_find_stream_info");
            return false;
        }

        if (inFmtCtx->nb_streams < 1) {
            FF_SET_CODE_S(AVERROR_UNKNOWN, "nb_streams < 1");
            return false;
        }

        return true;
    }

    void deInitInFormatCtx()
    {
        avformat_close_input(&inFmtCtx);
    }

    bool initOutFormatCtx(const std::string& outUrl)
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
        if (result < 0) {
            result = avformat_alloc_output_context2(&outFmtCtx, nullptr, nullptr, outUrl.c_str());
        }

        if (result < 0) {
            FF_SET_CODE_S(result, "avformat_alloc_output_context2");
            return false;
        }

        if (outFmtCtx == nullptr) {
            FF_SET_CODE_S(AVERROR_UNKNOWN, "avformat_alloc_output_context2");
            return false;
        }

        outFmtCtx->interrupt_callback = interruptCB.GetAVIOInterruptCB();
        outFmtCtx->max_interleave_delta = 1000;

        int streamIndex = 0;
        for (int i = 0; i < inFmtCtx->nb_streams; i++) {
            AVStream* oStream = nullptr;
            AVStream* iStream = inFmtCtx->streams[i];
            AVCodecParameters* iCodecPar = iStream->codecpar;

            if (iCodecPar->codec_type != AVMEDIA_TYPE_AUDIO && iCodecPar->codec_type != AVMEDIA_TYPE_VIDEO && iCodecPar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
                streamMapping.push_back(-1);
                continue;
            }

            streamMapping.push_back(streamIndex++);
            oStream = avformat_new_stream(outFmtCtx, nullptr);
            if (oStream == nullptr) {
                FF_SET_CODE_S(AVERROR_UNKNOWN, "avformat_new_stream");
                return false;
            }

            const int result = avcodec_parameters_copy(oStream->codecpar, iCodecPar);
            if (result < 0) {
                FF_SET_CODE_S(result, "avcodec_parameters_copy");
                return false;
            }

            oStream->codecpar->codec_tag = 0;
        }

        // av_dump_format(oFmtCtx, 0, outUrl.c_str(), 1);
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

    void deInitOutFormatCtx()
    {
        if (outFmtCtx != nullptr) {
            if (ioOpenResult >= 0) {
                av_write_trailer(outFmtCtx);
            }

            if ((outFmtCtx->pb != nullptr) && !(outFmtCtx->flags & AVFMT_NOFILE) && (ioOpenResult >= 0)) {
                avio_closep(&outFmtCtx->pb);
            }

            avformat_free_context(outFmtCtx);
            outFmtCtx = nullptr;
        }
    }

private:
    std::tuple<int, std::string, int> code = { 0, "", -1 };
    std::vector<int> streamMapping;
    FFInterruptCB interruptCB;
    int ioOpenResult = -1;
    int ioWriteHeadResult = -1;
    AVDictionary* options = nullptr;
    AVFormatContext* inFmtCtx = nullptr;
    AVFormatContext* outFmtCtx = nullptr;
};
