#pragma once

namespace Utils
{
    template <typename T>
    struct Smooth
    {
        Smooth(size_t sampleCount)
        {
            values.resize(sampleCount, T(0));
            total = (T)0.f;
            ring = 0;
        }
        
        void add(T& v)
        {
            size_t i = (ring % values.size());
            total += v - values[i];
            values[i] = v;
            ring++;
        }
        
        T	get()
        {
            return total / (T)(values.size());
        }
        
        std::vector<T>	values;
        size_t			ring;
        T				total;
    };
};