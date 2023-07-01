/*
 * MIT License
 *
 * Copyright (c) 2017-2019 Mikhail Pilin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <cstdint>

namespace ww898 {
namespace utf {

namespace detail {

// wrapper around a random-access iterator that reads a few elements at a time, no bounds check
template<typename It, typename TChar>
struct ReadIterRnd
{
    using InternalIt = It;
    ReadIterRnd(It init) :it(init) {}
    It it;

    It get() {
        return it;
    }
    auto read1()
    {
        const TChar a = *it++;
        return a;
    }
    auto read2()
    {
        const TChar a = *it;
        const TChar b = *(it + 1);
        it += 2;
        return std::tuple(a, b);
    }
    auto read3()
    {
        const TChar a = *it;
        const TChar b = *(it + 1);
        const TChar c = *(it + 2);
        it += 3;
        return std::tuple(a, b, c);
    }
    auto read4()
    {
        const TChar a = *it;
        const TChar b = *(it + 1);
        const TChar c = *(it + 2);
        const TChar d = *(it + 3);
        it += 4;
        return std::tuple(a, b, c, d);
    }
    auto read5()
    {
        const TChar a = *it;
        const TChar b = *(it + 1);
        const TChar c = *(it + 2);
        const TChar d = *(it + 3);
        const TChar e = *(it + 4);
        it += 5;
        return std::tuple(a, b, c, d, e);
    }
};

// explicit specialization for const char* that passes the result as uint32_t to generate slightly more optimized code
template<>
struct ReadIterRnd<const char*, uint8_t>
{
    using InternalIt = const uint8_t*;
    ReadIterRnd(const char* init) :it(reinterpret_cast<InternalIt>(init)) {}
    // internal representation needs to be unsigned to avoid sign-extension issues
    InternalIt it;

    const char* get() {
        return (const char*)it;
    }
    auto read1()
    {
        const uint32_t a = *it++;
        return a;
    }
    auto read2()
    {
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        it += 2;
        return std::tuple(a, b);
    }
    auto read3()
    {
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        const uint32_t c = *(it + 2);
        it += 3;
        return std::tuple(a, b, c);
    }
    auto read4()
    {
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        const uint32_t c = *(it + 2);
        const uint32_t d = *(it + 3);
        it += 4;
        return std::tuple(a, b, c, d);
    }
    auto read5()
    {
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        const uint32_t c = *(it + 2);
        const uint32_t d = *(it + 3);
        const uint32_t e = *(it + 4);
        it += 5;
        return std::tuple(a, b, c, d, e);
    }
};

// read iterator, random access, range checked with an end iterator
template<typename It, typename TChar>
struct ReadIterRndChecked
{
    using InternalIt = It;
    ReadIterRndChecked(It init, It einit) :it(init), eit(einit) {}
    It it, eit;

    auto read1()
    {
        if (it == eit)
            throw std::runtime_error("Not enough input");
        const TChar a = *it++;
        return a;
    }
    auto read2()
    {
        if (1 >= eit - it)
            throw std::runtime_error("Not enough input");
        const TChar a = *it;
        const TChar b = *(it + 1);
        it += 2;
        return std::tuple(a, b);
    }
    auto read3()
    {
        if (2 >= eit - it)
            throw std::runtime_error("Not enough input");
        const TChar a = *it;
        const TChar b = *(it + 1);
        const TChar c = *(it + 2);
        it += 3;
        return std::tuple(a, b, c);
    }
    auto read4()
    {
        if (3 >= eit - it)
            throw std::runtime_error("Not enough input");
        const TChar a = *it;
        const TChar b = *(it + 1);
        const TChar c = *(it + 2);
        const TChar d = *(it + 3);
        it += 4;
        return std::tuple(a, b, c, d);
    }
    auto read5()
    {
        if (4 >= eit - it)
            throw std::runtime_error("Not enough input");
        const TChar a = *it;
        const TChar b = *(it + 1);
        const TChar c = *(it + 2);
        const TChar d = *(it + 3);
        const TChar e = *(it + 4);
        it += 5;
        return std::tuple(a, b, c, d, e);
    }
};

// specialization to const char* iterator that does the above uint32_t expansion and also does addtion (LEA opcode) in the pointer arithmatics instead of subtraction (SUB opcode)
template<>
struct ReadIterRndChecked<const char*, uint8_t>
{
    using InternalIt = const uint8_t*;
    ReadIterRndChecked(const char* init, const char* einit) :it(reinterpret_cast<InternalIt>(init)), eit(reinterpret_cast<InternalIt>(einit)) {}
    InternalIt it, eit;

    auto read1()
    {
        if (it == eit)
            throw std::runtime_error("Not enough input");
        const uint32_t a = *it++;
        return a;
    }
    auto read2()
    {
        if (it + 1 >= eit)
            throw std::runtime_error("Not enough input");
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        it += 2;
        return std::tuple(a, b);
    }
    auto read3()
    {
        if (it + 2 >= eit)
            throw std::runtime_error("Not enough input");
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        const uint32_t c = *(it + 2);
        it += 3;
        return std::tuple(a, b, c);
    }
    auto read4()
    {
        if (it + 3 >= eit)
            throw std::runtime_error("Not enough input");
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        const uint32_t c = *(it + 2);
        const uint32_t d = *(it + 3);
        it += 4;
        return std::tuple(a, b, c, d);
    }
    auto read5()
    {
        if (it + 4 >= eit)
            throw std::runtime_error("Not enough input");
        const uint32_t a = *it;
        const uint32_t b = *(it + 1);
        const uint32_t c = *(it + 2);
        const uint32_t d = *(it + 3);
        const uint32_t e = *(it + 4);
        it += 5;
        return std::tuple(a, b, c, d, e);
    }
};

// read iterator, with null termination check
template<typename It, typename TChar>
struct ReadIterZChecked
{
    using InternalIt = It;
    ReadIterZChecked(It init) :it(init) {}
    It it;

    // legitimate null terminator is only read here
    auto read1()
    {
        const TChar a = *it++;
        return a;
    }

    auto read1_noz()
    {
        const TChar c = *it++;
        if (c == 0)
            throw std::runtime_error("Unexpected null termination");
        return c;
    }

    // if we're reading more than 1 element, none of them should be 0
    auto read2()
    {
        const TChar a = read1_noz();
        const TChar b = read1_noz();
        return std::tuple(a, b);
    }
    auto read3()
    {
        const TChar a = read1_noz();
        const TChar b = read1_noz();
        const TChar c = read1_noz();
        return std::tuple(a, b, c);
    }
    auto read4()
    {
        const TChar a = read1_noz();
        const TChar b = read1_noz();
        const TChar c = read1_noz();
        const TChar d = read1_noz();
        return std::tuple(a, b, c, d);
    }
    auto read5()
    {
        const TChar a = read1_noz();
        const TChar b = read1_noz();
        const TChar c = read1_noz();
        const TChar d = read1_noz();
        const TChar e = read1_noz();
        return std::tuple(a, b, c, d, e);
    }
};

// read wrapper for non-random-access iterator
template<typename It, typename TChar>
struct ReadIterSeqChecked
{
    using InternalIt = It;
    ReadIterSeqChecked(It init, It einit) :it(init), eit(einit) {}
    It it, eit;

    auto read1()
    {
        if (it == eit)
            throw std::runtime_error("Not enough input");
        const TChar a = *it++;
        return a;
    }

    auto read2()
    {
        const TChar a = read1();
        const TChar b = read1();
        return std::tuple(a, b);
    }
    auto read3()
    {
        const TChar a = read1();
        const TChar b = read1();
        const TChar c = read1();
        return std::tuple(a, b, c);
    }
    auto read4()
    {
        const TChar a = read1();
        const TChar b = read1();
        const TChar c = read1();
        const TChar d = read1();
        return std::tuple(a, b, c, d);
    }
    auto read5()
    {
        const TChar a = read1();
        const TChar b = read1();
        const TChar c = read1();
        const TChar d = read1();
        const TChar e = read1();
        return std::tuple(a, b, c, d, e);
    }
};


// write iterator wrapper
template<typename Oit, typename TChar>
struct WriteIter
{
    Oit oit;
    void write1(const TChar ch)
    {
        *oit++ = ch;
    }
    void write2(const TChar ch1, const TChar ch2)
    {
        *oit++ = ch1;
        *oit++ = ch2;
    }
    void write3(const TChar ch1, const TChar ch2, const TChar ch3)
    {
        *oit++ = ch1;
        *oit++ = ch2;
        *oit++ = ch3;
    }
    void write4(const TChar ch1, const TChar ch2, const TChar ch3, const TChar ch4)
    {
        *oit++ = ch1;
        *oit++ = ch2;
        *oit++ = ch3;
        *oit++ = ch4;
    }
    void write5(const TChar ch1, const TChar ch2, const TChar ch3, const TChar ch4, const TChar ch5)
    {
        *oit++ = ch1;
        *oit++ = ch2;
        *oit++ = ch3;
        *oit++ = ch4;
        *oit++ = ch5;
    }
    void write6(const TChar ch1, const TChar ch2, const TChar ch3, const TChar ch4, const TChar ch5, const TChar ch6)
    {
        *oit++ = ch1;
        *oit++ = ch2;
        *oit++ = ch3;
        *oit++ = ch4;
        *oit++ = ch5;
        *oit++ = ch6;
    }
};

}}}