#include "tool.h"
#include <cstdint>
#include <iostream>
#define __STDC_CONSTANT_MACROS
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
}
const size_t AUDIO_INBUF_SIZE = 20480;
const size_t AUDIO_REFILL_THRESH = 4096;
// Function to convert a file to PCM format using FFmpeg
bool convertToPCM(const std::string& inputFileName, const std::string& outputFileName, std::function<void(double)> update_progress_callback) {
    // Initialize FFmpeg
    size_t input_file_size = getFileSize(inputFileName.c_str());
    if (input_file_size == 0) {
        throw std::runtime_error("Invalid input file");
        return false;
    }
    // Open input file
    AVFormatContext* formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, inputFileName.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Error opening input file" << std::endl;
        return false;
    }
    
    // Find stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "Error finding stream information" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }

    // Find audio stream
    int audioStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }

    if (audioStreamIndex == -1) {
        std::cerr << "Error finding audio stream" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }

    // Find audio decoder
    const AVCodec* codec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
    if (!codec) {
        std::cerr << "Error finding audio decoder" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }

    // Allocate codec context
    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        std::cerr << "Error allocating codec context" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }

    // Copy codec parameters to context
    if (avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar) < 0) {
        std::cerr << "Error copying codec parameters" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }

    // Open codec
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cerr << "Error opening codec" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }

    // Allocate frame
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Error allocating frame" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }

    // Initialize resampler
    SwrContext* swrContext = swr_alloc();
    if (!swrContext) {
        std::cerr << "Error allocating resampler context" << std::endl;
        av_frame_free(&frame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }
    av_opt_set_int(swrContext, "in_channel_layout", codecContext->ch_layout.nb_channels, 0);
    av_opt_set_int(swrContext, "in_sample_rate", codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", codecContext->sample_fmt, 0);

    av_opt_set_int(swrContext, "out_channel_layout", codecContext->ch_layout.nb_channels, 0);
    av_opt_set_int(swrContext, "out_sample_rate", codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S64, 0);

    // Initialize resampler with options
    if (swr_init(swrContext) < 0) {
        std::cerr << "Error initializing resampler" << std::endl;
        swr_free(&swrContext);
        av_frame_free(&frame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }

    // Open output file
    FILE* outputFile = fopen(outputFileName.c_str(), "wb");
    if (!outputFile) {
        std::cerr << "Error opening output file" << std::endl;
        swr_free(&swrContext);
        av_frame_free(&frame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }
    
    // Allocate packet
    AVPacket packet;
    // Read frames and convert to PCM
    AVFrame *out_frame = av_frame_alloc();
    int out_nb_samples = av_rescale_rnd(2048, codecContext->sample_rate, codecContext->sample_rate, AV_ROUND_UP);
    av_samples_alloc(out_frame->data, out_frame->linesize, codecContext->ch_layout.nb_channels, out_nb_samples, AV_SAMPLE_FMT_S64, 1);
    size_t readed_size = 0;

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == audioStreamIndex) {
            // Decode audio packet
            if (avcodec_send_packet(codecContext, &packet) < 0) {
                std::cerr << "Error sending packet to decoder" << std::endl;
                break;
            }
            while (avcodec_receive_frame(codecContext, frame) == 0) {
                // Resample audio frame
                int dataSize = swr_convert(swrContext, (uint8_t**)&out_frame->data, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
                
                // Write resampled data to output file
                fwrite(out_frame->data[0], av_get_bytes_per_sample(AV_SAMPLE_FMT_S64)*codecContext->ch_layout.nb_channels, dataSize, outputFile);
                // Free resampled data
            }
            readed_size += packet.size;
        }
        if (update_progress_callback != nullptr) {
            double progress = static_cast<double>(readed_size) / static_cast<double>(input_file_size);
            update_progress_callback(progress);
        }
        // Free packet
        av_packet_unref(&packet);
    }
    if (update_progress_callback != nullptr) {
        update_progress_callback(2);    // avoid float precision problem
    }
    // Close output file
    fclose(outputFile);

    // Free resources
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);

    return true;
}