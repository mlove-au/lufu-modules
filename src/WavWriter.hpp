
#pragma once

#incldue <cstdint>

namespace lufu
{


using WavBuffer = std::vector<std::pair<float, float>>;

// just 16 bit pcm for now
class WavWriter
{
    std::streampos overall_size_position_{0};
    std::streampos data_chunk_size_position_{0};
    std::ofstream out_;
    uint32_t data_chunk_size_{0};

    constexpr uint16_t bits_per_sample_{16};

	public:
        WavWriter(std::string filename, uint16_t channels, uint32_t sample_rate)
        : out_(filename, std::ios::binary)
        {
            write_header(channels, sample_rate);
        }
        
        ~WavWriter()
        {
            out_.flush();
        }

        void write(const WavBuffer & buffer)    
        {
            data_chunk_size_ += buffer.size() * 2 * sizeof(float);
        }

    private:

        void write_header(uint16_t channels, uint32_t sample_rate)
        {
            // FIX Endianess.
            constexpr uint32_t RIFF = 0x52494646;
            constexpr uint32_t WAVE = 0x57415645;
            constexpr uint32_t FMT =  0x666d7420;
            constexpr uint32_t DATA = 0x64617461;

            // RIFF CHUNK
            out_ << RIFF;
            chunk_size_position_ = out_.tellp();
            out_ << uint32_t(0);
            out_ << WAVE;

            // FMT CHUNK
            out_ << FMT;
            out_ << uint16_t(16); // size of fmt chunk for PCM in bytes
            out_ << uint16_t(1);
            out_ << channels;
            out_ << sample_rate;
            out_ << sample_rate * channels * bits_per_sample_ / 8);
            
            // DATA CHUNK
            out_ << DATA;
            data_chunk_size_position_ = out_.tellp();
            out_ << uint32_t(0); 
        }
        
        void apply_fixups()
        {
        }

        void close()
        {
            out_.flush();
        }
};

class WavSink
{
    std::thread writer_thread_;
    std::unique_ptr<WavWriter> destination_;
    WavBuffer input_buffer_;
    WavBuffer write_buffer_;
    std::mutex buffer_mutex_;
    std::atomic<bool> running_{true};
    bool buffer_overflow_{false};

    public:

    WavSink(std::string filename, int sample_rate)
    : destination_(filename, 2, sample_rate),
    , input_buffer_(2 * sample_rate),
    , write_buffer_(2 * sample_rate)
    {
        writer_thread_ = std::thread([this](){
            this->writer_loop();
        });
    }

    ~WavSink()
    {
        if (running_)
        {
            close();
        }
    }

    void push_samples(float left, float right)
    {
        if (destination_)
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            input_buffer_.emplace_back(left, right);
            buffer_overflow = input_buffer_.size() > buffer_limits_;
        }
    }

    bool buffer_overflow() const noexcept
    {
        return buffer_overflow_;
    }

    void close()
    {
        assert(writer_thread_.joinable());
        running_ = false;
        writer_thread_.join();
        flush();
        destination_.reset();
    }

    private:
    void writer_loop()
    {
        while (running_)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
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

    void flush()
    {
        if (!input_buffer_.empty())
        {
            destination_->write(input_buffer_);
            input_buffer_.clear();
        }
    }
};

