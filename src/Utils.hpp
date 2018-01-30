#pragma once

#include <sstream>
#include <iomanip>
#include <vector>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 3)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}

template <typename T = rack::Label>
T * createLabel(Vec pos, std::string text)
{
    auto l = new T();
    l->box.pos = std::move(pos);
    l->text = std::move(text);
    return l;
}

// A simple ring-buffer type iterator over a container.
// Allows "analogue" type increments that are less than an
// entire integer increment.
template <typename Container>
struct cyclic_iterator
{
    using value_type = typename Container::value_type;
    using tick_type = float;

    static constexpr tick_type ONE_TICK{ 1.0 };
    const Container * container_;
    tick_type end_;
    tick_type idx_;

    cyclic_iterator()
    : container_(nullptr)
    , end_(0.0)
    , idx_(0.0)  
    {
    }

    cyclic_iterator(const Container & c)
    : container_(&c)
    , end_(c.size())
    , idx_(0.0)
    {
    }

    void reset(const Container & c)
    {
        *this = cyclic_iterator(c);
    }

    void reset()
    {
        *this = cyclic_iterator();
    }

    cyclic_iterator& operator=(const cyclic_iterator & that)
    {
        container_ = that.container_;
        end_ = that.end_;
        idx_ = that.idx_;
        return *this;        
    }

    cyclic_iterator(const cyclic_iterator & that)
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

    cyclic_iterator& operator+=(tick_type d) noexcept
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

    cyclic_iterator& operator++() noexcept
    {
        return this->operator+=(ONE_TICK);
    }

    size_t offset() const noexcept
    {
        return size_t(idx_);
    }
};

template <typename T>
using ref_vec = std::vector<std::reference_wrapper<T>>;

template <typename T> ref_vec<T>
wrap(T * data, size_t num)
{
    return std::vector<std::reference_wrapper<T>>(data, data + num);
}

template <typename T>
inline void set_all(ref_vec<T> & vec, T t)
{
    for (auto & v : vec)
    {
        v = t;
    }
}

