/*
 * MIT License
 * 
 * Copyright (c) 2017 Mikhail Pilin
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
#include <stdexcept>
#include <iterator>

namespace ww898 {
namespace utf {

static uint32_t const max_unicode_code_point = 0x10FFFF;

// Note: Only for UTF8 and UTF32 !!!
static uint32_t const max_supported_code_point = 0x7FFFFFFF;

static uint16_t const min_surrogate = 0xD800;
static uint16_t const max_surrogate = 0xDFFF;

static uint16_t const min_surrogate_high = 0xD800;
static uint16_t const max_surrogate_high = 0xDBFF;

static uint16_t const min_surrogate_low = 0xDC00;
static uint16_t const max_surrogate_low = 0xDFFF;

inline bool is_surrogate_high(uint32_t const cp)
{
    return min_surrogate_high <= cp && cp <= max_surrogate_high;
}

inline bool is_surrogate_low(uint32_t const cp)
{
    return min_surrogate_low <= cp && cp <= max_surrogate_low;
}

inline bool is_surrogate(uint32_t const cp)
{
    return min_surrogate <= cp && cp <= max_surrogate;
}

struct utf8 final
{
    static size_t const max_unicode_symbol_size = 4;
    static size_t const max_supported_symbol_size = 6;

    static uint32_t const max_code_point = max_supported_code_point;
    static_assert(max_code_point == (1u << 31) - 1u, "Invalid maximum supported code point");

    template<
        typename It>
    static size_t size(It & it)
    {
        uint8_t const chf = *it;
        if (chf < 0x80)
            return 1;
        else if (chf < 0xC0)
            throw std::runtime_error("Unexpected UTF8 slave symbol at master position");
        else if (chf < 0xE0)
            return 2;
        else if (chf < 0xF0)
            return 3;
        else if (chf < 0xF8)
            return 4;
        else if (chf < 0xFC)
            return 5;
        else if (chf < 0xFE)
            return 6;
        else
            throw std::runtime_error("Invalid UTF8 master symbol");
    }

    template<
        typename It,
        typename VerifyIt>
    static uint32_t read(It & it, VerifyIt && verify_it)
    {
        uint8_t const chf = *it++;
        if (chf < 0x80)      // 0xxx_xxxx
            return chf;
        else if (chf < 0xC0)
            throw std::runtime_error("Unexpected UTF8 slave symbol at master position");
        uint32_t cp;
        size_t extra;
        if (chf < 0xE0)      // 110x_xxxx 10xx_xxxx
        {
            cp = chf & 0x1F;
            extra = 1;
        }
        else if (chf < 0xF0) // 1110_xxxx 10xx_xxxx 10xx_xxxx
        {
            cp = chf & 0x0F;
            extra = 2;
        }
        else if (chf < 0xF8) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            cp = chf & 0x07;
            extra = 3;
        }
        else if (chf < 0xFC) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            cp = chf & 0x03;
            extra = 4;
        }
        else if (chf < 0xFE) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            cp = chf & 0x01;
            extra = 5;
        }
        else
            throw std::runtime_error("Invalid UTF8 master symbol");
        while (extra-- > 0)
        {
            verify_it();
            uint8_t const chn = *it++;
            if (chn < 0x80 || 0xC0 <= chn)
                throw std::runtime_error("Invalid UTF8 slave symbol");
            cp = (cp << 6) | (chn & 0x3F);
        }
        return cp;
    }

    template<
        typename Oit>
    static void write(uint32_t const cp, Oit & oit)
    {
        if (cp < 0x80)          // 0xxx_xxxx
            *oit++ = static_cast<uint8_t>(cp);
        else if (cp < 0x800)    // 110x_xxxx 10xx_xxxx
        {
            *oit++ = static_cast<uint8_t>(0xC0 | (cp >> 6       ));
            *oit++ = static_cast<uint8_t>(0x80 | (cp      & 0x3F));
        }
        else if (cp < 0x10000)  // 1110_xxxx 10xx_xxxx 10xx_xxxx
        {
            *oit++ = static_cast<uint8_t>(0xE0 | (cp >> 12       ));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >>  6 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp       & 0x3F));
        }
        else if (cp < 0x200000) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            *oit++ = static_cast<uint8_t>(0xF0 | (cp >> 18       ));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >> 12 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >>  6 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp       & 0x3F));
        }
        else if (cp < 0x4000000) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            *oit++ = static_cast<uint8_t>(0xF8 | (cp >> 24       ));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >> 18 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >> 12 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >>  6 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp       & 0x3F));
        }
        else if (cp < 0x80000000) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
        {
            *oit++ = static_cast<uint8_t>(0xFC | (cp >> 30       ));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >> 24 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >> 18 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >> 12 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp >>  6 & 0x3F));
            *oit++ = static_cast<uint8_t>(0x80 | (cp       & 0x3F));
        }
        else
            throw std::runtime_error("Unsupported UTF8 code point");
    }
};

struct utf16 final
{
    static size_t const max_unicode_symbol_size = 2;
    static size_t const max_supported_symbol_size = 2;

    static uint32_t const max_code_point = max_unicode_code_point;
    static_assert(max_code_point == 0x10000u + (1u << 20) - 1u, "Invalid maximum supported code point");

    template<
        typename It>
    static size_t size(It & it)
    {
        uint16_t const chf = *it;
        if (chf < 0xD800 || 0xE000 <= chf)
            return 1;
        else if (chf < 0xDC00)
            return 2;
        else
            throw std::runtime_error("Unexpected UTF16 slave symbol at master position");
    }

    template<
        typename It,
        typename VerifyIt>
    static uint32_t read(It & it, VerifyIt && verify_it)
    {
        uint16_t const chf = *it++;
        if (chf < 0xD800 || 0xE000 <= chf) // [0x0000‥0xD7FF] or [0xE000‥0xFFFF]
            return chf;
        else if (chf < 0xDC00)              // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
        {
            verify_it();
            uint16_t const chn = *it++;
            if (chn < 0xDC00 || 0xE000 <= chn)
                throw std::runtime_error("Invalid UTF16 slave symbol");
            return (
                static_cast<uint32_t>(chf - 0xD800) << 10 |
                static_cast<uint32_t>(chn - 0xDC00)       ) + 0x10000;
        }
        else
            throw std::runtime_error("Unexpected UTF16 slave symbol at master position");
    }

    template<
        typename Oit>
    static void write(uint32_t const cp, Oit & oit)
    {
        if (is_surrogate(cp))
            throw std::runtime_error("Surrogate code point detected");
        if (cp < 0x10000)       // [0x0000‥0xD7FF] or [0xE000‥0xFFFF]
            *oit++ = static_cast<uint16_t>(cp);
        else if (cp < 0x110000) // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
        {
            uint32_t const vl = cp - 0x10000;
            *oit++ = static_cast<uint16_t>(0xD800 + (vl >> 10        ));
            *oit++ = static_cast<uint16_t>(0xDC00 + (vl       & 0x3FF));
        }
        else
            throw std::runtime_error("Unsupported UTF16 code point");
    }
};

struct utf32 final
{
    static size_t const max_unicode_symbol_size = 1;
    static size_t const max_supported_symbol_size = 1;

    static uint32_t const max_code_point = max_supported_code_point;
    static_assert(max_code_point == (1u << 31) - 1u, "Invalid maximum supported code point");

    template<
        typename It>
    static size_t size(It &)
    {
        return 1;
    }

    template<
        typename It,
        typename VerifyIt>
    static uint32_t read(It & it, VerifyIt &&)
    {
        return *it++;
    }

    template<
        typename Oit>
    static void write(uint32_t const cp, Oit & oit)
    {
        if (cp < 0x80000000)
            *oit++ = cp;
        else
            throw std::runtime_error("Unsupported UTF32 code point");
    }
};

template<
    typename Utf,
    typename It>
size_t size(It it)
{
    return Utf::size(it);
}

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
It convz(It it, Oit oit)
{
    while (true)
    {
        auto const cp = Utf::read(it, [] {});
        if (!cp)
            return it;
        Outf::write(cp, oit);
    }
}

namespace detail {

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit,
    bool>
struct conv_strategy;

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct conv_strategy<Utf, Outf, It, Oit, false> final
{
    static void func(It it, It const eit, Oit oit)
    {
        while (it != eit)
            Outf::write(Utf::read(it, [&it, &eit]
                {
                    if (it == eit)
                        throw std::runtime_error("Not enough input");
                }), oit);
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct conv_strategy<Utf, Outf, It, Oit, true> final
{
    static void func(It it, It const eit, Oit oit)
    {
        if (eit - it >= Utf::max_supported_symbol_size)
        {
            auto const fast_eit = eit - Utf::max_supported_symbol_size;
            while (it < fast_eit)
                Outf::write(Utf::read(it, [] {}), oit);
        }
        while (it != eit)
            Outf::write(Utf::read(it, [&it, &eit]
                {
                    if (it == eit)
                        throw std::runtime_error("Not enough input");
                }), oit);
    }
};

}

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit,
    bool is_random_access_iterator = std::is_base_of<
        std::random_access_iterator_tag,
        typename std::iterator_traits<It>::iterator_category>::value>
void conv(It && it, It && eit, Oit && oit)
{
    detail::conv_strategy<Utf, Outf, It, Oit, is_random_access_iterator>::func(
        std::forward<It>(it),
        std::forward<It>(eit),
        std::forward<Oit>(oit));
}

}}
