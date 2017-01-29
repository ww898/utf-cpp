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

inline bool is_surrogate_high(uint32_t const _Cp)
{
	return min_surrogate_high <= _Cp && _Cp <= max_surrogate_high;
}

inline bool is_surrogate_low(uint32_t const _Cp)
{
	return min_surrogate_low <= _Cp && _Cp <= max_surrogate_low;
}

inline bool is_surrogate(uint32_t const _Cp)
{
	return min_surrogate <= _Cp && _Cp <= max_surrogate;
}

struct utf8 final
{
	static size_t const max_unicode_symbol_size = 4;
	static size_t const max_supported_symbol_size = 6;

	static uint32_t const max_code_point = max_supported_code_point;
	static_assert(max_code_point == (1u << 31) - 1u, "Invalid maximum supported code point");

	template<
		typename _It_t>
	static size_t size(_It_t & _It)
	{
		uint8_t const _Ch1 = *_It;
		if (_Ch1 < 0x80)
			return 1;
		else if (_Ch1 < 0xC0)
			throw std::runtime_error("Unexpected UTF8 slave symbol at master position");
		else if (_Ch1 < 0xE0)
			return 2;
		else if (_Ch1 < 0xF0)
			return 3;
		else if (_Ch1 < 0xF8)
			return 4;
		else if (_Ch1 < 0xFC)
			return 5;
		else if (_Ch1 < 0xFE)
			return 6;
		else
			throw std::runtime_error("Invalid UTF8 master symbol");
	}

	template<
		typename _It_t,
		typename _Vit_t>
	static uint32_t read(_It_t & _It, _Vit_t && _Vit)
	{
		uint8_t const _Ch1 = *_It++;
		if (_Ch1 < 0x80)      // 0xxx_xxxx
			return _Ch1;
		else if (_Ch1 < 0xC0)
			throw std::runtime_error("Unexpected UTF8 slave symbol at master position");
		uint32_t _Cp;
		size_t _Extra;
		if (_Ch1 < 0xE0)      // 110x_xxxx 10xx_xxxx
		{
			_Cp = _Ch1 & 0x1F;
			_Extra = 1;
		}
		else if (_Ch1 < 0xF0) // 1110_xxxx 10xx_xxxx 10xx_xxxx
		{
			_Cp = _Ch1 & 0x0F;
			_Extra = 2;
		}
		else if (_Ch1 < 0xF8) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
		{
			_Cp = _Ch1 & 0x07;
			_Extra = 3;
		}
		else if (_Ch1 < 0xFC) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
		{
			_Cp = _Ch1 & 0x03;
			_Extra = 4;
		}
		else if (_Ch1 < 0xFE) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
		{
			_Cp = _Ch1 & 0x01;
			_Extra = 5;
		}
		else
			throw std::runtime_error("Invalid UTF8 master symbol");
		while (_Extra-- > 0)
		{
			_Vit();
			uint8_t const _Ch = *_It++;
			if (_Ch < 0x80 || 0xC0 <= _Ch)
				throw std::runtime_error("Invalid UTF8 slave symbol");
			_Cp = (_Cp << 6) | (_Ch & 0x3F);
		}
		return _Cp;
	}

	template<
		typename _Oit_t>
	static void write(uint32_t const _Cp, _Oit_t & _Oit)
	{
		if (_Cp < 0x80)          // 0xxx_xxxx
			*_Oit++ = static_cast<uint8_t>(_Cp);
		else if (_Cp < 0x800)    // 110x_xxxx 10xx_xxxx
		{
			*_Oit++ = static_cast<uint8_t>(0xC0 | (_Cp >> 6       ));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp      & 0x3F));
		}
		else if (_Cp < 0x10000)  // 1110_xxxx 10xx_xxxx 10xx_xxxx
		{
			*_Oit++ = static_cast<uint8_t>(0xE0 | (_Cp >> 12       ));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >>  6 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp       & 0x3F));
		}
		else if (_Cp < 0x200000) // 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
		{
			*_Oit++ = static_cast<uint8_t>(0xF0 | (_Cp >> 18       ));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >> 12 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >>  6 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp       & 0x3F));
		}
		else if (_Cp < 0x4000000) // 1111_10xx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
		{
			*_Oit++ = static_cast<uint8_t>(0xF8 | (_Cp >> 24       ));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >> 18 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >> 12 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >>  6 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp       & 0x3F));
		}
		else if (_Cp < 0x80000000) // 1111_110x 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
		{
			*_Oit++ = static_cast<uint8_t>(0xFC | (_Cp >> 30       ));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >> 24 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >> 18 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >> 12 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp >>  6 & 0x3F));
			*_Oit++ = static_cast<uint8_t>(0x80 | (_Cp       & 0x3F));
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
		typename _It_t>
	static size_t size(_It_t & _It)
	{
		uint16_t const _Ch = *_It;
		if (_Ch < 0xD800 || 0xE000 <= _Ch)
			return 1;
		else if (_Ch < 0xDC00)
			return 2;
		else
			throw std::runtime_error("Unexpected UTF16 slave symbol at master position");
	}

	template<
		typename _It_t,
		typename _Vit_t>
	static uint32_t read(_It_t & _It, _Vit_t && _Vit)
	{
		uint16_t const _Ch1 = *_It++;
		if (_Ch1 < 0xD800 || 0xE000 <= _Ch1) // [0x0000‥0xD7FF] or [0xE000‥0xFFFF]
			return _Ch1;
		else if (_Ch1 < 0xDC00)              // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
		{
			_Vit();
			uint16_t const _Ch2 = *_It++;
			if (_Ch2 < 0xDC00 || 0xE000 <= _Ch2)
				throw std::runtime_error("Invalid UTF16 slave symbol");
			return (
				static_cast<uint32_t>(_Ch1 - 0xD800) << 10 |
				static_cast<uint32_t>(_Ch2 - 0xDC00)       ) + 0x10000;
		}
		else
			throw std::runtime_error("Unexpected UTF16 slave symbol at master position");
	}

	template<
		typename _Oit_t>
	static void write(uint32_t const _Cp, _Oit_t & _Oit)
	{
		if (is_surrogate(_Cp))
			throw std::runtime_error("Surrogate code point detected");
		if (_Cp < 0x10000)       // [0x0000‥0xD7FF] or [0xE000‥0xFFFF]
			*_Oit++ = static_cast<uint16_t>(_Cp);
		else if (_Cp < 0x110000) // [0xD800‥0xDBFF] [0xDC00‥0xDFFF]
		{
			uint32_t const _Vl = _Cp - 0x10000;
			*_Oit++ = static_cast<uint16_t>(0xD800 + (_Vl >> 10        ));
			*_Oit++ = static_cast<uint16_t>(0xDC00 + (_Vl       & 0x3FF));
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
		typename _It_t>
	static size_t size(_It_t &)
	{
		return 1;
	}

	template<
		typename _It_t,
		typename _Vit_t>
	static uint32_t read(_It_t & _It, _Vit_t &&)
	{
		return *_It++;
	}

	template<
		typename _Oit_t>
	static void write(uint32_t const _Cp, _Oit_t & _Oit)
	{
		if (_Cp < 0x80000000)
			*_Oit++ = _Cp;
		else
			throw std::runtime_error("Unsupported UTF32 code point");
	}
};

template<
	typename _Utf_t,
	typename _It_t>
size_t size(_It_t _It)
{
	return _Utf_t::size(_It);
}

template<
	typename _Utf_t,
	typename _Outf_t,
	typename _It_t,
	typename _Oit_t>
_It_t convz(_It_t _It, _Oit_t _Oit)
{
	while (true)
	{
		auto const _Cp = _Utf_t::read(_It, [] {});
		if (!_Cp)
			return _It;
		_Outf_t::write(_Cp, _Oit);
	}
}

namespace detail {

template<
	typename _Utf_t,
	typename _Outf_t,
	typename _It_t,
	typename _Oit_t,
	bool>
struct conv_strategy;

template<
	typename _Utf_t,
	typename _Outf_t,
	typename _It_t,
	typename _Oit_t>
struct conv_strategy<_Utf_t, _Outf_t, _It_t, _Oit_t, false> final
{
	static void func(_It_t _It, _It_t const _Eit, _Oit_t _Oit)
	{
		while (_It != _Eit)
			_Outf_t::write(_Utf_t::read(_It, [&_It, &_Eit]
				{
					if (_It == _Eit)
						throw std::runtime_error("Not enough input");
				}), _Oit);
	}
};

template<
	typename _Utf_t,
	typename _Outf_t,
	typename _It_t,
	typename _Oit_t>
struct conv_strategy<_Utf_t, _Outf_t, _It_t, _Oit_t, true> final
{
	static void func(_It_t _It, _It_t const _Eit, _Oit_t _Oit)
	{
		auto const _Unsafe_eit = _Eit - _Utf_t::max_supported_symbol_size;
		while (_It < _Unsafe_eit)
			_Outf_t::write(_Utf_t::read(_It, [] {}), _Oit);
		while (_It != _Eit)
			_Outf_t::write(_Utf_t::read(_It, [&_It, &_Eit]
				{
					if (_It == _Eit)
						throw std::runtime_error("Not enough input");
				}), _Oit);
	}
};

}

template<
	typename _Utf_t,
	typename _Outf_t,
	typename _It_t,
	typename _Oit_t,
	bool _Is_random_access_iterator = std::is_base_of<
		std::random_access_iterator_tag,
		typename std::iterator_traits<_It_t>::iterator_category>::value>
void conv(_It_t && _It, _It_t && _Eit, _Oit_t && _Oit)
{
	detail::conv_strategy<_Utf_t, _Outf_t, _It_t, _Oit_t, _Is_random_access_iterator>::func(
		std::forward<_It_t>(_It),
		std::forward<_It_t>(_Eit),
		std::forward<_Oit_t>(_Oit));
}

}}
