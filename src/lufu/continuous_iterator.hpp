
#pragma once

#include <sstream>
#include <iomanip>
#include <vector>

namespace lufu
{

// A simple ring-buffer type iterator over a vector container 
// that loops around to stay within the container range.
// Allows "analogue" type increments that are less than an
// entire integer increment.
template <typename Container>
struct continuous_iterator
{
    using value_type = typename Container::value_type;
    using tick_type = float;

    static constexpr tick_type ONE_TICK{ 1.0 };
    const Container * container_;
    tick_type end_;
    tick_type idx_;

    continuous_iterator()
        : container_(nullptr)
        , end_(0.0)
        , idx_(0.0)
    {
    }

    continuous_iterator(const Container & c)
        : container_(&c)
        , end_(c.size())
        , idx_(0.0)
    {
    }

    void reset(const Container & c)
    {
        *this = continuous_iterator(c);
    }

    void reset()
    {
        *this = continuous_iterator();
    }

    continuous_iterator& operator=(const continuous_iterator & that)
    {
        container_ = that.container_;
        end_ = that.end_;
        idx_ = that.idx_;
        return *this;
    }

    continuous_iterator(const continuous_iterator & that)
        : container_(that.container_)
        , end_(that.end_)
        , idx_(that.idx_)
    {
    }

    operator bool() const noexcept
    {
        return container_ != nullptr;
    }

    value_type operator*() const
    {
        assert(container_ != nullptr);
        return (*container_)[offset()];
    }

    continuous_iterator& operator+=(tick_type d) noexcept
    {
        idx_ += d;
        if (idx_ < 0.0)
        {
            idx_ = end_ + idx_;
        }
        else if (idx_ >= end_)
        {
            idx_ = idx_ - end_;
        }
        return *this;
    }

    continuous_iterator& operator++() noexcept
    {
        return this->operator+=(ONE_TICK);
    }

    size_t offset() const noexcept
    {
        return size_t(idx_);
    }
};

}