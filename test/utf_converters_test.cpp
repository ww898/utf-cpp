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

#include <ww898/utf_converters.hpp>

#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(WW898_BOOST_TEST_INCLUDED)
#include <boost/test/included/unit_test.hpp>
#else
#include <boost/test/unit_test.hpp>
#endif

#include <boost/test/data/test_case.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <codecvt>

#if defined(__linux__)
#include <chrono>
#endif

namespace ww898 {
namespace test {

BOOST_AUTO_TEST_SUITE(utf_converters)

namespace {

struct utf_tuple_unicode final
{
	std::string utf8;
	std::wstring utf16;
	std::u32string utf32;
};

struct utf_tuple_supported final
{
	std::string utf8;
	std::u32string utf32;
};

utf_tuple_unicode const _Ourpairs_unicode[] =
{
	{
		"\x24",
		L"\x0024",
		{
			0x00000024
		}
	}, // $

	{
		"\xC2\xA2",
		L"\x00A2",
		{
			0x000000A2
		}
	}, // ¢

	{
		"\xE2\x82\xAC",
		L"\x20AC",
		{
			0x000020AC
		}
	}, // €

	{
		"\xF0\x90\x8D\x88",
		L"\xD800\xDF48",
		{
			0x00010348
		}
	}, // 𐍈

	{
		"\xF0\xA4\xAD\xA2",
		L"\xD852\xDF62",
		{
			0x00024B62
		}
	}, // 𤭢

	{
		"\xF0\x90\x90\xB7",
		L"\xD801\xDC37",
		{
			0x00010437
		}
	}, // 𐐷

	{
		"\xEF\xAB\x82",
		L"\xFAC2",
		{
			0x0000FAC2
		}
	}, // 輸

	{
		"\xD0\xAE\xD0\xBD\xD0\xB8\xD0\xBA\xD0\xBE\xD0\xB4",
		L"\x042E\x043D\x0438\x043A\x043E\x0434",
		{
			0x0000042E, 0x0000043D, 0x00000438, 0x0000043A, 0x0000043E, 0x00000434
		}
	}, // Юникод

	{
		"\xC5\xAA\x6E\xC4\xAD\x63\xC5\x8D\x64\x65\xCC\xBD",
		L"\x016A\x006E\x012D\x0063\x014D\x0064\x0065\x033D",
		{
			0x0000016A, 0x0000006E, 0x0000012D, 0x00000063, 0x0000014D, 0x00000064, 0x00000065, 0x0000033D
		}
	}, // Ūnĭcōde̽

	{
		"\xE0\xA4\xAF\xE0\xA5\x82\xE0\xA4\xA8\xE0\xA4\xBF\xE0\xA4\x95\xE0\xA5\x8B\xE0\xA4\xA1",
		L"\x092F\x0942\x0928\x093F\x0915\x094B\x0921",
		{
			0x0000092F, 0x00000942, 0x00000928, 0x0000093F, 0x00000915, 0x0000094B, 0x00000921
		}
	}, // यूनिकोड

	{
		"\x41\xE2\x89\xA2\xCE\x91\x2E",
		L"\x0041\x2262\x0391\x002E",
		{
			0x00000041, 0x00002262, 0x00000391, 0x0000002E
		}
	}, // A≢Α.

	{
		"\xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4",
		L"\xD55C\xAD6D\xC5B4",
		{
			0x0000D55C, 0x0000AD6D, 0x0000C5B4
		}
	}, // 한국어

	{
		"\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E",
		L"\x65E5\x672C\x8A9E",
		{
			0x000065E5, 0x0000672C, 0x00008A9E
		}
	}, // 日本語

	{
		"\xE1\x9B\x81\xE1\x9A\xB3\xE1\x9B\xAB\xE1\x9B\x97\xE1\x9A\xA8\xE1\x9A\xB7\xE1\x9B\xAB\xE1\x9A\xB7\xE1\x9B\x9A\xE1\x9A\xA8\xE1\x9B\x8B\xE1\x9B"
		"\xAB\xE1\x9B\x96\xE1\x9A\xA9\xE1\x9B\x8F\xE1\x9A\xAA\xE1\x9A\xBE\xE1\x9B\xAB\xE1\x9A\xA9\xE1\x9A\xBE\xE1\x9B\x9E\xE1\x9B\xAB\xE1\x9A\xBB\xE1"
		"\x9B\x81\xE1\x9B\x8F\xE1\x9B\xAB\xE1\x9A\xBE\xE1\x9B\x96\xE1\x9B\xAB\xE1\x9A\xBB\xE1\x9B\x96\xE1\x9A\xAA\xE1\x9A\xB1\xE1\x9B\x97\xE1\x9B\x81"
		"\xE1\x9A\xAA\xE1\x9A\xA7\xE1\x9B\xAB\xE1\x9B\x97\xE1\x9B\x96\xE1\x9B\xAC",
		L"\x16C1\x16B3\x16EB\x16D7\x16A8\x16B7\x16EB\x16B7\x16DA\x16A8\x16CB\x16EB\x16D6\x16A9\x16CF\x16AA\x16BE\x16EB\x16A9\x16BE\x16DE\x16EB\x16BB"
		L"\x16C1\x16CF\x16EB\x16BE\x16D6\x16EB\x16BB\x16D6\x16AA\x16B1\x16D7\x16C1\x16AA\x16A7\x16EB\x16D7\x16D6\x16EC",
		{
			0x000016C1, 0x000016B3, 0x000016EB, 0x000016D7, 0x000016A8, 0x000016B7, 0x000016EB, 0x000016B7, 0x000016DA, 0x000016A8, 0x000016CB, 0x000016EB,
			0x000016D6, 0x000016A9, 0x000016CF, 0x000016AA, 0x000016BE, 0x000016EB, 0x000016A9, 0x000016BE, 0x000016DE, 0x000016EB, 0x000016BB, 0x000016C1,
			0x000016CF, 0x000016EB, 0x000016BE, 0x000016D6, 0x000016EB, 0x000016BB, 0x000016D6, 0x000016AA, 0x000016B1, 0x000016D7, 0x000016C1, 0x000016AA,
			0x000016A7, 0x000016EB, 0x000016D7, 0x000016D6, 0x000016EC
		}
	}, // ᛁᚳ᛫ᛗᚨᚷ᛫ᚷᛚᚨᛋ᛫ᛖᚩᛏᚪᚾ᛫ᚩᚾᛞ᛫ᚻᛁᛏ᛫ᚾᛖ᛫ᚻᛖᚪᚱᛗᛁᚪᚧ᛫ᛗᛖ᛬

	{
		"\xE1\x9A\x9B\xE1\x9A\x9B\xE1\x9A\x89\xE1\x9A\x91\xE1\x9A\x85\xE1\x9A\x94\xE1\x9A\x89\xE1\x9A\x89\xE1\x9A\x94\xE1\x9A\x8B\xE1\x9A\x80\xE1\x9A"
		"\x94\xE1\x9A\x88\xE1\x9A\x94\xE1\x9A\x80\xE1\x9A\x8D\xE1\x9A\x82\xE1\x9A\x90\xE1\x9A\x85\xE1\x9A\x91\xE1\x9A\x80\xE1\x9A\x85\xE1\x9A\x94\xE1"
		"\x9A\x8B\xE1\x9A\x8C\xE1\x9A\x93\xE1\x9A\x85\xE1\x9A\x90\xE1\x9A\x9C",
		L"\x169B\x169B\x1689\x1691\x1685\x1694\x1689\x1689\x1694\x168B\x1680\x1694\x1688\x1694\x1680\x168D\x1682\x1690\x1685\x1691\x1680\x1685\x1694"
		L"\x168B\x168C\x1693\x1685\x1690\x169C",
		{
			0x0000169B, 0x0000169B, 0x00001689, 0x00001691, 0x00001685, 0x00001694, 0x00001689, 0x00001689, 0x00001694, 0x0000168B, 0x00001680, 0x00001694,
			0x00001688, 0x00001694, 0x00001680, 0x0000168D, 0x00001682, 0x00001690, 0x00001685, 0x00001691, 0x00001680, 0x00001685, 0x00001694, 0x0000168B,
			0x0000168C, 0x00001693, 0x00001685, 0x00001690, 0x0000169C
		}
	}, // ᚛᚛ᚉᚑᚅᚔᚉᚉᚔᚋ ᚔᚈᚔ ᚍᚂᚐᚅᚑ ᚅᚔᚋᚌᚓᚅᚐ᚜

	{
		"\xE2\xA0\x8A\xE2\xA0\x80\xE2\xA0\x89\xE2\xA0\x81\xE2\xA0\x9D\xE2\xA0\x80\xE2\xA0\x91\xE2\xA0\x81\xE2\xA0\x9E\xE2\xA0\x80\xE2\xA0\x9B\xE2\xA0"
		"\x87\xE2\xA0\x81\xE2\xA0\x8E\xE2\xA0\x8E\xE2\xA0\x80\xE2\xA0\x81\xE2\xA0\x9D\xE2\xA0\x99\xE2\xA0\x80\xE2\xA0\x8A\xE2\xA0\x9E\xE2\xA0\x80\xE2"
		"\xA0\x99\xE2\xA0\x95\xE2\xA0\x91\xE2\xA0\x8E\xE2\xA0\x9D\xE2\xA0\x9E\xE2\xA0\x80\xE2\xA0\x93\xE2\xA0\xA5\xE2\xA0\x97\xE2\xA0\x9E\xE2\xA0\x80"
		"\xE2\xA0\x8D\xE2\xA0\x91",
		L"\x280A\x2800\x2809\x2801\x281D\x2800\x2811\x2801\x281E\x2800\x281B\x2807\x2801\x280E\x280E\x2800\x2801\x281D\x2819\x2800\x280A\x281E\x2800"
		L"\x2819\x2815\x2811\x280E\x281D\x281E\x2800\x2813\x2825\x2817\x281E\x2800\x280D\x2811",
		{
			0x0000280A, 0x00002800, 0x00002809, 0x00002801, 0x0000281D, 0x00002800, 0x00002811, 0x00002801, 0x0000281E, 0x00002800, 0x0000281B, 0x00002807,
			0x00002801, 0x0000280E, 0x0000280E, 0x00002800, 0x00002801, 0x0000281D, 0x00002819, 0x00002800, 0x0000280A, 0x0000281E, 0x00002800, 0x00002819,
			0x00002815, 0x00002811, 0x0000280E, 0x0000281D, 0x0000281E, 0x00002800, 0x00002813, 0x00002825, 0x00002817, 0x0000281E, 0x00002800, 0x0000280D,
			0x00002811
		}
	}, // ⠊⠀⠉⠁⠝⠀⠑⠁⠞⠀⠛⠇⠁⠎⠎⠀⠁⠝⠙⠀⠊⠞⠀⠙⠕⠑⠎⠝⠞⠀⠓⠥⠗⠞⠀⠍⠑

	{
		"\xD8\xA3\xD9\x86\xD8\xA7\x20\xD9\x82\xD8\xA7\xD8\xAF\xD8\xB1\x20\xD8\xB9\xD9\x84\xD9\x89\x20\xD8\xA3\xD9\x83\xD9\x84\x20\xD8\xA7\xD9\x84\xD8"
		"\xB2\xD8\xAC\xD8\xA7\xD8\xAC\x20\xD9\x88\x20\xD9\x87\xD8\xB0\xD8\xA7\x20\xD9\x84\xD8\xA7\x20\xD9\x8A\xD8\xA4\xD9\x84\xD9\x85\xD9\x86\xD9\x8A"
		"\x2E",
		L"\x0623\x0646\x0627\x0020\x0642\x0627\x062F\x0631\x0020\x0639\x0644\x0649\x0020\x0623\x0643\x0644\x0020\x0627\x0644\x0632\x062C\x0627\x062C"
		L"\x0020\x0648\x0020\x0647\x0630\x0627\x0020\x0644\x0627\x0020\x064A\x0624\x0644\x0645\x0646\x064A\x002E",
		{
			0x00000623, 0x00000646, 0x00000627, 0x00000020, 0x00000642, 0x00000627, 0x0000062F, 0x00000631, 0x00000020, 0x00000639, 0x00000644, 0x00000649,
			0x00000020, 0x00000623, 0x00000643, 0x00000644, 0x00000020, 0x00000627, 0x00000644, 0x00000632, 0x0000062C, 0x00000627, 0x0000062C, 0x00000020,
			0x00000648, 0x00000020, 0x00000647, 0x00000630, 0x00000627, 0x00000020, 0x00000644, 0x00000627, 0x00000020, 0x0000064A, 0x00000624, 0x00000644,
			0x00000645, 0x00000646, 0x0000064A, 0x0000002E
		}
	}, // أنا قادر على أكل الزجاج و هذا لا يؤلمني.

	{
		"\xE1\x80\x80\xE1\x80\xB9\xE1\x80\x9A\xE1\x80\xB9\xE1\x80\x9D\xE1\x80\x94\xE1\x80\xB9\xE2\x80\x8C\xE1\x80\x90\xE1\x80\xB1\xE1\x80\xAC\xE1\x80"
		"\xB9\xE2\x80\x8C\xE1\x81\x8A\xE1\x80\x80\xE1\x80\xB9\xE1\x80\x9A\xE1\x80\xB9\xE1\x80\x9D\xE1\x80\x94\xE1\x80\xB9\xE2\x80\x8C\xE1\x80\x99\x20"
		"\xE1\x80\x99\xE1\x80\xB9\xE1\x80\x9A\xE1\x80\x80\xE1\x80\xB9\xE2\x80\x8C\xE1\x80\x85\xE1\x80\xAC\xE1\x80\xB8\xE1\x80\x94\xE1\x80\xAF\xE1\x80"
		"\xAD\xE1\x80\x84\xE1\x80\xB9\xE2\x80\x8C\xE1\x80\x9E\xE1\x80\x8A\xE1\x80\xB9\xE2\x80\x8C\xE1\x81\x8B\x20\xE1\x81\x8E\xE1\x80\x80\xE1\x80\xB9"
		"\xE1\x80\x9B\xE1\x80\xB1\xE1\x80\xAC\xE1\x80\x84\xE1\x80\xB9\xE2\x80\x8C\xE1\x80\xB7\x20\xE1\x80\x91\xE1\x80\xAD\xE1\x80\x81\xE1\x80\xAF\xE1"
		"\x80\xAD\xE1\x80\x80\xE1\x80\xB9\xE2\x80\x8C\xE1\x80\x99\xE1\x80\xB9\xE1\x80\x9F\xE1\x80\xAF\x20\xE1\x80\x99\xE1\x80\x9B\xE1\x80\xB9\xE1\x80"
		"\x9F\xE1\x80\xAD\xE1\x80\x95\xE1\x80\xAC\xE1\x81\x8B",
		L"\x1000\x1039\x101A\x1039\x101D\x1014\x1039\x200C\x1010\x1031\x102C\x1039\x200C\x104A\x1000\x1039\x101A\x1039\x101D\x1014\x1039\x200C\x1019"
		L"\x0020\x1019\x1039\x101A\x1000\x1039\x200C\x1005\x102C\x1038\x1014\x102F\x102D\x1004\x1039\x200C\x101E\x100A\x1039\x200C\x104B\x0020\x104E"
		L"\x1000\x1039\x101B\x1031\x102C\x1004\x1039\x200C\x1037\x0020\x1011\x102D\x1001\x102F\x102D\x1000\x1039\x200C\x1019\x1039\x101F\x102F\x0020"
		L"\x1019\x101B\x1039\x101F\x102D\x1015\x102C\x104B",
		{
			0x00001000, 0x00001039, 0x0000101A, 0x00001039, 0x0000101D, 0x00001014, 0x00001039, 0x0000200C, 0x00001010, 0x00001031, 0x0000102C, 0x00001039,
			0x0000200C, 0x0000104A, 0x00001000, 0x00001039, 0x0000101A, 0x00001039, 0x0000101D, 0x00001014, 0x00001039, 0x0000200C, 0x00001019, 0x00000020,
			0x00001019, 0x00001039, 0x0000101A, 0x00001000, 0x00001039, 0x0000200C, 0x00001005, 0x0000102C, 0x00001038, 0x00001014, 0x0000102F, 0x0000102D,
			0x00001004, 0x00001039, 0x0000200C, 0x0000101E, 0x0000100A, 0x00001039, 0x0000200C, 0x0000104B, 0x00000020, 0x0000104E, 0x00001000, 0x00001039,
			0x0000101B, 0x00001031, 0x0000102C, 0x00001004, 0x00001039, 0x0000200C, 0x00001037, 0x00000020, 0x00001011, 0x0000102D, 0x00001001, 0x0000102F,
			0x0000102D, 0x00001000, 0x00001039, 0x0000200C, 0x00001019, 0x00001039, 0x0000101F, 0x0000102F, 0x00000020, 0x00001019, 0x0000101B, 0x00001039,
			0x0000101F, 0x0000102D, 0x00001015, 0x0000102C, 0x0000104B
		}
	}, // က္ယ္ဝန္‌တော္‌၊က္ယ္ဝန္‌မ မ္ယက္‌စားနုိင္‌သည္‌။ ၎က္ရောင္‌့ ထိခုိက္‌မ္ဟု မရ္ဟိပာ။

	{
		"\xF0\x9F\xA0\x80\xF0\x9F\xA0\x81\xF0\x9F\xA0\x82\xF0\x9F\xA0\x83\xF0\x9F\xA0\x84\xF0\x9F\xA0\x85\xF0\x9F\xA0\x86\xF0\x9F\xA0\x87\xF0\x9F\xA0"
		"\x88\xF0\x9F\xA0\x89\xF0\x9F\xA0\x8A\xF0\x9F\xA0\x8B",
		L"\xD83E\xDC00\xD83E\xDC01\xD83E\xDC02\xD83E\xDC03\xD83E\xDC04\xD83E\xDC05\xD83E\xDC06\xD83E\xDC07\xD83E\xDC08\xD83E\xDC09\xD83E\xDC0A\xD83E"
		L"\xDC0B",
		{
			0x0001F800, 0x0001F801, 0x0001F802, 0x0001F803, 0x0001F804, 0x0001F805, 0x0001F806, 0x0001F807, 0x0001F808, 0x0001F809, 0x0001F80A, 0x0001F80B
		}
	}, // 🠀🠁🠂🠃🠄🠅🠆🠇🠈🠉🠊🠋

	{
		"\xF0\x9F\x80\x80\xF0\x9F\x80\x81\xF0\x9F\x80\x82\xF0\x9F\x80\x83\xF0\x9F\x80\x84\xF0\x9F\x80\x85\xF0\x9F\x80\x86\xF0\x9F\x80\x87\xF0\x9F\x80"
		"\x88\xF0\x9F\x80\x89\xF0\x9F\x80\x8A\xF0\x9F\x80\x8B\xF0\x9F\x80\x8C\xF0\x9F\x80\x8D\xF0\x9F\x80\x8E\xF0\x9F\x80\x8F\xF0\x9F\x80\x90\xF0\x9F"
		"\x80\x91\xF0\x9F\x80\x92\xF0\x9F\x80\x93\xF0\x9F\x80\x94\xF0\x9F\x80\x95\xF0\x9F\x80\x96\xF0\x9F\x80\x97\xF0\x9F\x80\x98\xF0\x9F\x80\x99\xF0"
		"\x9F\x80\x9A\xF0\x9F\x80\x9B\xF0\x9F\x80\x9C\xF0\x9F\x80\x9D\xF0\x9F\x80\x9E\xF0\x9F\x80\x9F\xF0\x9F\x80\xA0\xF0\x9F\x80\xA1\xF0\x9F\x80\xA2"
		"\xF0\x9F\x80\xA3\xF0\x9F\x80\xA4\xF0\x9F\x80\xA5\xF0\x9F\x80\xA6\xF0\x9F\x80\xA7\xF0\x9F\x80\xA8\xF0\x9F\x80\xA9\xF0\x9F\x80\xAA\xF0\x9F\x80"
		"\xAB",
		L"\xD83C\xDC00\xD83C\xDC01\xD83C\xDC02\xD83C\xDC03\xD83C\xDC04\xD83C\xDC05\xD83C\xDC06\xD83C\xDC07\xD83C\xDC08\xD83C\xDC09\xD83C\xDC0A\xD83C"
		L"\xDC0B\xD83C\xDC0C\xD83C\xDC0D\xD83C\xDC0E\xD83C\xDC0F\xD83C\xDC10\xD83C\xDC11\xD83C\xDC12\xD83C\xDC13\xD83C\xDC14\xD83C\xDC15\xD83C\xDC16"
		L"\xD83C\xDC17\xD83C\xDC18\xD83C\xDC19\xD83C\xDC1A\xD83C\xDC1B\xD83C\xDC1C\xD83C\xDC1D\xD83C\xDC1E\xD83C\xDC1F\xD83C\xDC20\xD83C\xDC21\xD83C"
		L"\xDC22\xD83C\xDC23\xD83C\xDC24\xD83C\xDC25\xD83C\xDC26\xD83C\xDC27\xD83C\xDC28\xD83C\xDC29\xD83C\xDC2A\xD83C\xDC2B",
		{
			0x0001F000, 0x0001F001, 0x0001F002, 0x0001F003, 0x0001F004, 0x0001F005, 0x0001F006, 0x0001F007, 0x0001F008, 0x0001F009, 0x0001F00A, 0x0001F00B,
			0x0001F00C, 0x0001F00D, 0x0001F00E, 0x0001F00F, 0x0001F010, 0x0001F011, 0x0001F012, 0x0001F013, 0x0001F014, 0x0001F015, 0x0001F016, 0x0001F017,
			0x0001F018, 0x0001F019, 0x0001F01A, 0x0001F01B, 0x0001F01C, 0x0001F01D, 0x0001F01E, 0x0001F01F, 0x0001F020, 0x0001F021, 0x0001F022, 0x0001F023,
			0x0001F024, 0x0001F025, 0x0001F026, 0x0001F027, 0x0001F028, 0x0001F029, 0x0001F02A, 0x0001F02B
		}
	}, // 🀀🀁🀂🀃🀄🀅🀆🀇🀈🀉🀊🀋🀌🀍🀎🀏🀐🀑🀒🀓🀔🀕🀖🀗🀘🀙🀚🀛🀜🀝🀞🀟🀠🀡🀢🀣🀤🀥🀦🀧🀨🀩🀪🀫

	{
		"\xED\x9F\xBF",
		L"\xD7FF",
		{
			utf::min_surrogate - 1
		}
	},

	{
		"\xEE\x80\x80",
		L"\xE000",
		{	
			utf::max_surrogate + 1
		}
	},

	{
		"\xF0\x90\x80\x80",
		L"\xD800\xDC00",
		{
			0x00010000
		}
	},

	{
		"\xF4\x8F\xBF\xBF",
		L"\xDBFF\xDFFF",
		{
			utf::max_unicode_code_point
		}
	},
};

utf_tuple_supported const _Ourpairs_supported[] =
{
	{
		"\xFA\x95\xA9\xB6\x83",
		{
			0x02569D83
		}
	},

	{
		"\xFD\x95\xA9\xB6\x83\xAC",
		{
			0x55A760EC
		}
	},
};

}

BOOST_DATA_TEST_CASE(conv_utf8_to_utf16, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	std::wstring _Res;
	auto const _It = _Tuple.utf8.c_str();
	auto const _Eit = utf::convz<utf::utf8, utf::utf16>(_It, std::back_inserter(_Res)) - 1;
	std::wstring _Res2;
	utf::conv<utf::utf8, utf::utf16>(_Tuple.utf8.cbegin(), _Tuple.utf8.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf16 == _Res &&
		_Tuple.utf16 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::string(_It, _Eit) == _Tuple.utf8;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf8_to_utf32, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	std::u32string _Res;
	auto const _It = _Tuple.utf8.c_str();
	auto const _Eit = utf::convz<utf::utf8, utf::utf32>(_It, std::back_inserter(_Res)) - 1;
	std::u32string _Res2;
	utf::conv<utf::utf8, utf::utf32>(_Tuple.utf8.cbegin(), _Tuple.utf8.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf32 == _Res &&
		_Tuple.utf32 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::string(_It, _Eit) == _Tuple.utf8;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf16_to_utf8, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	std::string _Res;
	auto const _It = _Tuple.utf16.c_str();
	auto const _Eit = utf::convz<utf::utf16, utf::utf8>(_It, std::back_inserter(_Res)) - 1;
	std::string _Res2;
	utf::conv<utf::utf16, utf::utf8>(_Tuple.utf16.cbegin(), _Tuple.utf16.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf8 == _Res &&
		_Tuple.utf8 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::wstring(_It, _Eit) == _Tuple.utf16;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf16_to_utf32, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	std::u32string _Res;
	auto const _It = _Tuple.utf16.c_str();
	auto const _Eit = utf::convz<utf::utf16, utf::utf32>(_It, std::back_inserter(_Res)) - 1;
	std::u32string _Res2;
	utf::conv<utf::utf16, utf::utf32>(_Tuple.utf16.cbegin(), _Tuple.utf16.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf32 == _Res &&
		_Tuple.utf32 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::wstring(_It, _Eit) == _Tuple.utf16;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf32_to_utf8, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	std::string _Res;
	auto const _It = _Tuple.utf32.c_str();
	auto const _Eit = utf::convz<utf::utf32, utf::utf8>(_It, std::back_inserter(_Res)) - 1;
	std::string _Res2;
	utf::conv<utf::utf32, utf::utf8>(_Tuple.utf32.cbegin(), _Tuple.utf32.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf8 == _Res &&
		_Tuple.utf8 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::u32string(_It, _Eit) == _Tuple.utf32;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf32_to_utf16, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	std::wstring _Res;
	auto const _It = _Tuple.utf32.c_str();
	auto const _Eit = utf::convz<utf::utf32, utf::utf16>(_It, std::back_inserter(_Res)) - 1;
	std::wstring _Res2;
	utf::conv<utf::utf32, utf::utf16>(_Tuple.utf32.cbegin(), _Tuple.utf32.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf16 == _Res &&
		_Tuple.utf16 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::u32string(_It, _Eit) == _Tuple.utf32;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf32_to_utf8_supported, boost::make_iterator_range(_Ourpairs_supported), _Tuple)
{
	std::string _Res;
	auto const _It = _Tuple.utf32.c_str();
	auto const _Eit = utf::convz<utf::utf32, utf::utf8>(_It, std::back_inserter(_Res)) - 1;
	std::string _Res2;
	utf::conv<utf::utf32, utf::utf8>(_Tuple.utf32.cbegin(), _Tuple.utf32.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf8 == _Res &&
		_Tuple.utf8 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::u32string(_It, _Eit) == _Tuple.utf32;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(conv_utf8_to_utf32_supported, boost::make_iterator_range(_Ourpairs_supported), _Tuple)
{
	std::u32string _Res;
	auto const _It = _Tuple.utf8.c_str();
	auto const _Eit = utf::convz<utf::utf8, utf::utf32>(_It, std::back_inserter(_Res)) - 1;
	std::u32string _Res2;
	utf::conv<utf::utf8, utf::utf32>(_Tuple.utf8.cbegin(), _Tuple.utf8.cend(), std::back_inserter(_Res2));
	auto const _Success =
		_Tuple.utf32 == _Res &&
		_Tuple.utf32 == _Res2;
	BOOST_TEST_REQUIRE(_Success);
	auto const _Success2 = std::string(_It, _Eit) == _Tuple.utf8;
	BOOST_TEST_REQUIRE(_Success2);
}

BOOST_DATA_TEST_CASE(size_utf8, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	static auto const max_symbol_size = utf::utf8::max_unicode_symbol_size;
	std::string _Res;
	for (auto _Str = _Tuple.utf8.c_str(); *_Str;)
	{
		auto const _Size = utf::size<utf::utf8>(_Str);
		BOOST_TEST_REQUIRE(!!_Size);
		BOOST_TEST_REQUIRE(_Size <= max_symbol_size);
		auto const _End_str = _Str + _Size;
		_Res += std::string(_Str, _End_str);
		_Str = _End_str;
	}
	auto const _Success = _Res == _Tuple.utf8;
	BOOST_TEST_REQUIRE(_Success);
}

BOOST_DATA_TEST_CASE(size_utf16, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	static auto const max_symbol_size = utf::utf16::max_unicode_symbol_size;
	std::wstring _Res;
	for (auto _Str = _Tuple.utf16.c_str(); *_Str;)
	{
		auto const _Size = utf::size<utf::utf16>(_Str);
		BOOST_TEST_REQUIRE(!!_Size);
		BOOST_TEST_REQUIRE(_Size <= max_symbol_size);
		auto const _End_str = _Str + _Size;
		_Res += std::wstring(_Str, _End_str);
		_Str = _End_str;
		
	}
	auto const _Success = _Res == _Tuple.utf16;
	BOOST_TEST_REQUIRE(_Success);
}

BOOST_DATA_TEST_CASE(size_utf32, boost::make_iterator_range(_Ourpairs_unicode), _Tuple)
{
	static auto const max_symbol_size = utf::utf32::max_unicode_symbol_size;
	std::u32string _Res;
	for (auto _Str = _Tuple.utf32.c_str(); *_Str;)
	{
		auto const _Size = utf::size<utf::utf32>(_Str);
		BOOST_TEST_REQUIRE(!!_Size);
		BOOST_TEST_REQUIRE(_Size <= max_symbol_size);
		auto const _End_str = _Str + _Size;
		_Res += std::u32string(_Str, _End_str);
		_Str = _End_str;
		
	}
	auto const _Success = _Res == _Tuple.utf32;
	BOOST_TEST_REQUIRE(_Success);
}

BOOST_DATA_TEST_CASE(size_utf8_supported, boost::make_iterator_range(_Ourpairs_supported), _Tuple)
{
	static auto const max_symbol_size = utf::utf8::max_supported_symbol_size;
	std::string _Res;
	for (auto _Str = _Tuple.utf8.c_str(); *_Str;)
	{
		auto const _Size = utf::size<utf::utf8>(_Str);
		BOOST_TEST_REQUIRE(!!_Size);
		BOOST_TEST_REQUIRE(_Size <= max_symbol_size);
		auto const _End_str = _Str + _Size;
		_Res += std::string(_Str, _End_str);
		_Str = _End_str;
	}
	auto const _Success = _Res == _Tuple.utf8;
	BOOST_TEST_REQUIRE(_Success);
}

BOOST_DATA_TEST_CASE(size_utf32_supported, boost::make_iterator_range(_Ourpairs_supported), _Tuple)
{
	static auto const max_symbol_size = utf::utf32::max_supported_symbol_size;
	std::u32string _Res;
	for (auto _Str = _Tuple.utf32.c_str(); *_Str;)
	{
		auto const _Size = utf::size<utf::utf32>(_Str);
		BOOST_TEST_REQUIRE(!!_Size);
		BOOST_TEST_REQUIRE(_Size <= max_symbol_size);
		auto const _End_str = _Str + _Size;
		_Res += std::u32string(_Str, _End_str);
		_Str = _End_str;
	}
	auto const _Success = _Res == _Tuple.utf32;
	BOOST_TEST_REQUIRE(_Success);
}

namespace {

inline uint64_t get_time() throw()
{
#if defined(_WIN32)

#if !defined(_M_IX86) && !defined(_M_X64)
	#error Unknown platform
#endif

	// Note: CPUID can be executed at any privilege level to serialize instruction execution. Serializing instruction
	//       execution guarantees that any modifications to flags, registers, and memory for previous instructions are
	//       completed before the next instruction is fetched and executed.
	int _Cpu_info[4];
	__cpuid(_Cpu_info, 0);
	return __rdtsc();

#elif defined(__GNUC__) || defined(__clang__)

#ifdef __i386__
	#define DIRTY "%ebx", "%ecx"
#elif __x86_64__
	#define DIRTY "%rbx", "%rcx"
#else
	#error Unknown platform
#endif

	uint32_t _Lo, _Hi;
	asm volatile(
		"cpuid\n\t"
		"rdtsc\n\t"
		: "=a"(_Lo), "=d"(_Hi)
		: "a"(0)
		: DIRTY);
	return static_cast<uint64_t>(_Hi) << 32 | _Lo;

#undef DIRTY

#else
#error get_time() is not implmeneted for this architecture
#endif
}

inline uint64_t get_time_resolution() throw()
{
#if defined(_WIN32)
	LARGE_INTEGER _Beg_orig, _End_orig, _Freq_orig;
	if (!QueryPerformanceCounter(&_Beg_orig))
		std::abort();
	auto const _Start_time = get_time();
	Sleep(1000);
	if (!QueryPerformanceCounter(&_End_orig))
		std::abort();
	auto const _End_time = get_time();
	if (!QueryPerformanceFrequency(&_Freq_orig))
		std::abort();
	auto const _Elapsed_orig = _End_orig.QuadPart - _Beg_orig.QuadPart;
	auto const _Elapsed_time = _End_time - _Start_time;
	return (_Freq_orig.QuadPart * _Elapsed_time + _Elapsed_orig / 2) / _Elapsed_orig;
#elif defined(__linux__)
	timespec _Beg_orig, _End_orig;
	if (clock_gettime(CLOCK_REALTIME, &_Beg_orig))
		std::abort();
	auto const _Start_time = get_time();
	sleep(1);
	if (clock_gettime(CLOCK_REALTIME, &_End_orig))
		std::abort();
	auto const _End_time = get_time();
	std::chrono::nanoseconds const _Elapsed_orig =
		(std::chrono::seconds(_End_orig.tv_sec) + std::chrono::nanoseconds(_End_orig.tv_nsec)) -
		(std::chrono::seconds(_Beg_orig.tv_sec) + std::chrono::nanoseconds(_Beg_orig.tv_nsec));
	auto const _Elapsed_time = _End_time - _Start_time;
	return static_cast<uint64_t>(_Elapsed_time * 1000000000ull / _Elapsed_orig.count());
#else
#error get_time() is not implmeneted for this architecture
#endif
}

}

#if defined(WW898_ENABLE_PERFORMANCE_TESTS)
#define WW898_PERFORMANCE_TESTS_MODE *boost::unit_test::enabled()
#else
#define WW898_PERFORMANCE_TESTS_MODE *boost::unit_test::disabled()
#endif

BOOST_AUTO_TEST_CASE(performance, WW898_PERFORMANCE_TESTS_MODE)
{
	static size_t const utf32_max_size = 16 * 1024 * 1024;
	static size_t const utf16_max_size = utf32_max_size * utf::utf16::max_unicode_symbol_size;
	static size_t const utf8_max_size = utf32_max_size * utf::utf8::max_unicode_symbol_size;
	static size_t const iterations = 16;

	std::vector<char> _Utf8;
	std::vector<wchar_t> _Utf16;
	std::vector<char32_t> _Utf32;

	_Utf8.reserve(utf8_max_size);
	_Utf16.reserve(utf16_max_size);
	_Utf16.reserve(utf32_max_size);

	boost::random::mt19937 _Random(0);
	for (auto _N = utf32_max_size; _N-- > 0; )
	{
		auto _Cp = _Random() % (utf::max_unicode_code_point + 1);
		if (utf::is_surrogate(_Cp))
			_Cp -= utf::min_surrogate;

#if defined(__GNUC__)
		// Bug: std::codecvt_utf8_utf16 for GCC v5.4.0 for Ubuntu 16.04 LTS failed to convert \xFFFF code point correctly.
		if (_Cp == 0xFFFF)
			_Cp = 0xFEFF;
#endif

		_Utf32.push_back(_Cp);
	}

	auto const _Resolution = get_time_resolution();
	std::cout << (boost::format("Resolution: %u") % _Resolution).str() << std::endl;

	double _Utf8_utf16_duration;
	double _Utf16_utf8_duration;

	{
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Utf8.clear();
			auto const _Start_time = get_time();
			utf::conv<utf::utf32, utf::utf8>(&_Utf32.front(), &_Utf32.back() + 1, std::back_inserter(_Utf8));
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		std::cout << (boost::format("UTF32 ==> UTF8 : %fs") % _Duration).str() << std::endl;
	}

	{
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Utf16.clear();
			auto const _Start_time = get_time();
			utf::conv<utf::utf32, utf::utf16>(&_Utf32.front(), &_Utf32.back() + 1, std::back_inserter(_Utf16));
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		std::cout << (boost::format("UTF32 ==> UTF16: %fs") % _Duration).str() << std::endl;
	}

	{
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Utf8.clear();
			auto const _Start_time = get_time();
			utf::conv<utf::utf16, utf::utf8>(&_Utf16.front(), &_Utf16.back() + 1, std::back_inserter(_Utf8));
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		_Utf16_utf8_duration = _Duration;
		std::cout << (boost::format("UTF16 ==> UTF8 : %fs") % _Duration).str() << std::endl;
	}

	{
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Utf32.clear();
			auto const _Start_time = get_time();
			utf::conv<utf::utf16, utf::utf32>(&_Utf16.front(), &_Utf16.back() + 1, std::back_inserter(_Utf32));
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		std::cout << (boost::format("UTF16 ==> UTF32: %fs") % _Duration).str() << std::endl;
	}

	{
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Utf16.clear();
			auto const _Start_time = get_time();
			utf::conv<utf::utf8, utf::utf16>(&_Utf8.front(), &_Utf8.back() + 1, std::back_inserter(_Utf16));
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		_Utf8_utf16_duration = _Duration;
		std::cout << (boost::format("UTF8  ==> UTF16: %fs") % _Duration).str() << std::endl;
	}

	{
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Utf32.clear();
			auto const _Start_time = get_time();
			utf::conv<utf::utf8, utf::utf32>(&_Utf8.front(), &_Utf8.back() + 1, std::back_inserter(_Utf32));
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		std::cout << (boost::format("UTF8  ==> UTF32: %fs") % _Duration).str() << std::endl;
	}

	std::cout << "codecvt_utf8_utf16<wchar_t>:" << std::endl;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t
#if defined(__GNUC__)
		// Bug: The std::little_endian should be selected directly in std::codecvt_utf8_utf16 for GCC v5.4.0 for Ubuntu 16.04 LTS.
		, ww898::utf::max_unicode_code_point
		, std::little_endian
#endif
		>> _Conv;

	{
		std::string _Res;
		_Res.reserve(utf8_max_size);
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Res.clear();
			auto const _Start_time = get_time();
			_Res = _Conv.to_bytes(&_Utf16.front(), &_Utf16.back() + 1);
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		BOOST_TEST_REQUIRE(_Res.size() == _Utf8.size());
		auto const _Same = memcmp(&_Utf8.front(), &*_Res.cbegin(), _Utf8.size()) == 0;
		BOOST_TEST_REQUIRE(_Same);
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		std::cout << (boost::format("UTF16 ==> UTF8 : %fs (%+.2f%%)") % _Duration % (100 * (_Duration / _Utf16_utf8_duration - 1))).str() << std::endl;
	}

	{
		std::wstring _Res;
		_Res.reserve(utf16_max_size);
		uint64_t _Sum_duration = 0;
		for (auto _N = iterations; _N-- > 0;)
		{
			_Res.clear();
			auto const _Start_time = get_time();
			_Res = _Conv.from_bytes(&_Utf8.front(), &_Utf8.back() + 1);
			auto const _End_time = get_time();
			_Sum_duration += _End_time - _Start_time;
		}
		BOOST_TEST_REQUIRE(_Res.size() == _Utf16.size());
		auto const _Same = memcmp(&_Utf16.front(), &*_Res.cbegin(), sizeof(wchar_t) * _Utf16.size()) == 0;
		BOOST_TEST_REQUIRE(_Same);
		auto const _Duration = static_cast<double>(_Sum_duration) / iterations / _Resolution;
		std::cout << (boost::format("UTF8  ==> UTF16: %fs (%+.2f%%)") % _Duration % (100 * (_Duration / _Utf8_utf16_duration - 1))).str() << std::endl;
	}
}

#undef WW898_PERFORMANCE_TESTS_MODE

BOOST_AUTO_TEST_SUITE_END()

}}

BOOST_TEST_DONT_PRINT_LOG_VALUE(ww898::test::utf_converters::utf_tuple_unicode)
BOOST_TEST_DONT_PRINT_LOG_VALUE(ww898::test::utf_converters::utf_tuple_supported)
