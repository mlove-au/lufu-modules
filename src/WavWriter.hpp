#pragma once

#include <cstdint>
#include <thread>
#include <mutex>
#include <memory>
#include <string>
#include <atomic>
#include <vector>
#include <utility>

namespace lufu
{
    class WavWriter;

    using WavBuffer = std::vector<std::pair<float, float>>;;

    class WavSink
    {
        std::thread writer_thread_;
        std::unique_ptr<WavWriter> destination_;
        WavBuffer input_buffer_;
        WavBuffer write_buffer_;
        std::mutex buffer_mutex_;
        std::atomic<bool> running_{ true };
        bool buffer_overflow_{ false };
        const size_t buffer_limits_;

    public:
        WavSink(std::string filename, int sample_rate);

        ~WavSink();

        void push_samples(float left, float right);

        bool buffer_overflow() const noexcept
        {
            return buffer_overflow_;
        }

        void close();

    private:
        void writer_loop();
        void flush();

    };
}

