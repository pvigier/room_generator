#pragma once

#include <cassert>
#include <vector>

namespace room
{

template<typename T>
class Array2
{
public:
    Array2(std::size_t height = 0, std::size_t width = 0)
    {
        reshape(height, width);
    }

    Array2(std::size_t height, std::size_t width, const T& defaultValue)
    {
        reshape(height, width, defaultValue);
    }

    // Using decltype(auto) allows the methods to work equally well with std::vector<bool>
    decltype(auto) get(std::size_t i, std::size_t j)
    {
        assert(i < mHeight && j < mWidth);
        return mData[i * mWidth + j];
    }

    decltype(auto) get(std::size_t i, std::size_t j) const
    {
        assert(i < mHeight && j < mWidth);
        return mData[i * mWidth + j];
    }

    void reshape(std::size_t height, std::size_t width)
    {
        mHeight = height;
        mWidth = width;
        mData.resize(mHeight * mWidth);
    }

    void reshape(std::size_t height, std::size_t width, const T& defaultValue)
    {
        mHeight = height;
        mWidth = width;
        mData.resize(mHeight * mWidth, defaultValue);
    }

    std::size_t getHeight() const
    {
        return mHeight;
    }

    std::size_t getWidth() const
    {
        return mWidth;
    }

private:
    std::size_t mHeight;
    std::size_t mWidth;
    std::vector<T> mData;
};

}