
#include "WavWriter.hpp"
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>

namespace lufu
{

// just 16 bit pcm for now
class WavWriter
{
    std::streampos overall_size_position_{ 0 };
    std::streampos data_chunk_size_position_{ 0 };
    std::ofstream out_;
    uint32_t    data_chunk_size_{ 0 };

    static constexpr uint16_t bits_per_sample_{ 16 };

public:
    WavWriter(std::string filename, uint16_t channels, uint32_t sample_rate)
        : out_(filename, std::ios::binary)
    {
        write_header(channels, sample_rate);
    }

    ~WavWriter()
    {
        apply_fixups();
        close();
    }

    inline int16_t normalize_and_clip(float sample)
    {
        int32_t s = (sample / 5.0) * 32768;
        if (s >= 32767)
        {
            s = 32767;
        }

        if (s <= -32768)
        {
            s = -32768;
        }
        return int16_t(s);
    }

    void write(const WavBuffer & buffer)
    {
        data_chunk_size_ += buffer.size() * 2 * sizeof(uint16_t);
        for (const auto & sample : buffer)
        {
            write(out_, normalize_and_clip(sample.first));
            write(out_, normalize_and_clip(sample.second));
        }
    }

private:
    template <typename T>
    void write(std::ofstream & out, const T & that)
    {
        out.write(reinterpret_cast<const char *>(&that), sizeof(T));
    }

    // constexpr me.
     uint32_t big_endian(uint32_t source)
    {
        auto s = reinterpret_cast<uint8_t*>(&source);
        return uint32_t(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
    }

    inline uint32_t little_endian(uint32_t source)
    {
        return source;
    }

    void write_header(uint16_t channels, uint32_t sample_rate)
    {
        // a few magic numbers, just the character representation
        // packed into 32 bits (host order).
        constexpr uint32_t RIFF = 0x52494646;
        constexpr uint32_t WAVE = 0x57415645;
        constexpr uint32_t FMT  = 0x666d7420;
        constexpr uint32_t DATA = 0x64617461;

        // RIFF CHUNK
        write(out_, big_endian(RIFF));
        overall_size_position_ = out_.tellp();
        write(out_, little_endian(uint32_t(0)));

        write(out_, big_endian(WAVE));

        // FMT CHUNK
        write(out_, big_endian(FMT));
        write(out_, uint32_t(16)); // size of fmt chunk for PCM in bytes
        write(out_, uint16_t(1));
        write(out_, channels);
        write(out_, sample_rate);
        write(out_, sample_rate * channels * bits_per_sample_ / 8);
        write(out_, uint16_t(channels * bits_per_sample_ / 8));
        write(out_, bits_per_sample_);
        // DATA CHUNK
        write(out_, big_endian(DATA));
        data_chunk_size_position_ = out_.tellp();
        write(out_, little_endian(uint32_t(0)));
    }

    void apply_fixups()
    {
        out_.seekp(data_chunk_size_position_);
        write(out_, little_endian(data_chunk_size_));
        out_.seekp(overall_size_position_);
        write(out_, little_endian(data_chunk_size_ + 36));
    }

    void close()
    {
        out_.flush();
    }
};

constexpr uint16_t WavWriter::bits_per_sample_; 


WavSink::WavSink(std::string filename, int sample_rate)
    : destination_(new WavWriter(filename, 2, sample_rate))
    , buffer_limits_(2 * sample_rate)
{
    input_buffer_.reserve(2 * sample_rate);
    write_buffer_.reserve(2 * sample_rate);
    writer_thread_ = std::thread([this]()
    {
        writer_loop();
    });
}

WavSink::~WavSink()
{
    if (running_)
    {
        close();
    }
}

void WavSink::push_samples(float left, float right)
{
    if (destination_)
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        input_buffer_.emplace_back(left, right);
        buffer_overflow_ = input_buffer_.size() > buffer_limits_;
    }
}


void WavSink::close()
{
    running_ = false;
    if (writer_thread_.joinable())
    {
        writer_thread_.join();
    }
    flush();
    destination_.reset();
}


void WavSink::writer_loop()
{
    while (running_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            std::swap(input_buffer_, write_buffer_);
            input_buffer_.clear();
        }
        
        if (!write_buffer_.empty())
        {    
            destination_->write(write_buffer_);
            write_buffer_.clear();
        }
    }
}

void WavSink::flush()
{
    if (!input_buffer_.empty())
    {
        destination_->write(input_buffer_);
        input_buffer_.clear();
    }
}

}

