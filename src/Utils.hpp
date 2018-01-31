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

