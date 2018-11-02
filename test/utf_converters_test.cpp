/*
 * MIT License
 * 
 * Copyright (c) 2017-2018 Mikhail Pilin
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
 * THE SOFTWARE IS PROVIDED "AS IS{ WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if defined(_WIN32)
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#endif

#include <ww898/utf_converters.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <intrin.h>
#endif

#if defined(WW898_BOOST_TEST_INCLUDED)
#include <boost/test/included/unit_test.hpp>
#else
#include <boost/test/unit_test.hpp>
#endif

#include <boost/test/data/test_case.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/range/iterator_range.hpp>

#include <iostream>
#include <iomanip>
#include <codecvt>

#if defined(__linux__) || defined(__APPLE__)
#include <chrono>

#if defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#endif

namespace ww898 {
namespace test {

BOOST_AUTO_TEST_SUITE(utf_converters)

namespace {

struct unicode_tuple final
{
    std::string    u8 ;
    std::u16string u16;
    std::u32string u32;
    std::wstring   uw ;
};

struct supported_tuple final
{
    std::string    u8 ;
    std::u32string u32;
};

unicode_tuple const unicode_test_data[] =
{
    {
        { '\x24' },
        { 0x0024 },
        { 0x00000024 },
        L"$"
    },

    {
        { '\xC2', '\xA2' },
        { 0x00A2 },
        { 0x000000A2 },
        L"¢"
    },

    {
        { '\xE2', '\x82', '\xAC' },
        { 0x20AC },
        { 0x000020AC },
        L"€"
    },

    {
        { '\xF0', '\x90', '\x8D', '\x88' },
        { 0xD800, 0xDF48 },
        { 0x00010348 },
        L"𐍈"
    },

    {
        { '\xF0', '\xA4', '\xAD', '\xA2' },
        { 0xD852, 0xDF62 },
        { 0x00024B62 },
        L"𤭢"
    },

    {
        { '\xF0', '\x90', '\x90', '\xB7' },
        { 0xD801, 0xDC37 },
        { 0x00010437 },
        L"𐐷"
    },

    {
        { '\xEF', '\xAB', '\x82' },
        { 0xFAC2 },
        { 0x0000FAC2 },
        L"輸"
    },

    {
        { '\xD0', '\xAE', '\xD0', '\xBD', '\xD0', '\xB8', '\xD0', '\xBA', '\xD0', '\xBE', '\xD0', '\xB4' },
        { 0x042E, 0x043D, 0x0438, 0x043A, 0x043E, 0x0434 },
        { 0x0000042E, 0x0000043D, 0x00000438, 0x0000043A, 0x0000043E, 0x00000434 },
        L"Юникод"
    },

    {
        { '\xC5', '\xAA', '\x6E', '\xC4', '\xAD', '\x63', '\xC5', '\x8D', '\x64', '\x65', '\xCC', '\xBD' },
        { 0x016A, 0x006E, 0x012D, 0x0063, 0x014D, 0x0064, 0x0065, 0x033D },
        { 0x0000016A, 0x0000006E, 0x0000012D, 0x00000063, 0x0000014D, 0x00000064, 0x00000065, 0x0000033D },
        L"Ūnĭcōde̽"
    },

    {
        {
            '\xE0', '\xA4', '\xAF', '\xE0', '\xA5', '\x82', '\xE0', '\xA4', '\xA8', '\xE0', '\xA4', '\xBF', '\xE0', '\xA4', '\x95', '\xE0', '\xA5', '\x8B',
            '\xE0', '\xA4', '\xA1'
        },
        { 0x092F, 0x0942, 0x0928, 0x093F, 0x0915, 0x094B, 0x0921 },
        { 0x0000092F, 0x00000942, 0x00000928, 0x0000093F, 0x00000915, 0x0000094B, 0x00000921 },
        L"यूनिकोड"
    },

    {
        { '\x41', '\xE2', '\x89', '\xA2', '\xCE', '\x91', '\x2E' },
        { 0x0041, 0x2262, 0x0391, 0x002E },
        { 0x00000041, 0x00002262, 0x00000391, 0x0000002E },
        L"A≢Α."
    },

    {
        { '\xED', '\x95', '\x9C', '\xEA', '\xB5', '\xAD', '\xEC', '\x96', '\xB4' },
        { 0xD55C, 0xAD6D, 0xC5B4 },
        { 0x0000D55C, 0x0000AD6D, 0x0000C5B4 },
        L"한국어"
    },

    {
        { '\xE6', '\x97', '\xA5', '\xE6', '\x9C', '\xAC', '\xE8', '\xAA', '\x9E' },
        { 0x65E5, 0x672C, 0x8A9E },
        { 0x000065E5, 0x0000672C, 0x00008A9E },
        L"日本語"
    },

    {
        {
            '\xE1', '\x9B', '\x81', '\xE1', '\x9A', '\xB3', '\xE1', '\x9B', '\xAB', '\xE1', '\x9B', '\x97', '\xE1', '\x9A', '\xA8', '\xE1', '\x9A', '\xB7',
            '\xE1', '\x9B', '\xAB', '\xE1', '\x9A', '\xB7', '\xE1', '\x9B', '\x9A', '\xE1', '\x9A', '\xA8', '\xE1', '\x9B', '\x8B', '\xE1', '\x9B', '\xAB',
            '\xE1', '\x9B', '\x96', '\xE1', '\x9A', '\xA9', '\xE1', '\x9B', '\x8F', '\xE1', '\x9A', '\xAA', '\xE1', '\x9A', '\xBE', '\xE1', '\x9B', '\xAB',
            '\xE1', '\x9A', '\xA9', '\xE1', '\x9A', '\xBE', '\xE1', '\x9B', '\x9E', '\xE1', '\x9B', '\xAB', '\xE1', '\x9A', '\xBB', '\xE1', '\x9B', '\x81',
            '\xE1', '\x9B', '\x8F', '\xE1', '\x9B', '\xAB', '\xE1', '\x9A', '\xBE', '\xE1', '\x9B', '\x96', '\xE1', '\x9B', '\xAB', '\xE1', '\x9A', '\xBB',
            '\xE1', '\x9B', '\x96', '\xE1', '\x9A', '\xAA', '\xE1', '\x9A', '\xB1', '\xE1', '\x9B', '\x97', '\xE1', '\x9B', '\x81', '\xE1', '\x9A', '\xAA',
            '\xE1', '\x9A', '\xA7', '\xE1', '\x9B', '\xAB', '\xE1', '\x9B', '\x97', '\xE1', '\x9B', '\x96', '\xE1', '\x9B', '\xAC'
        },
        {
            0x16C1, 0x16B3, 0x16EB, 0x16D7, 0x16A8, 0x16B7, 0x16EB, 0x16B7, 0x16DA, 0x16A8, 0x16CB, 0x16EB, 0x16D6, 0x16A9, 0x16CF, 0x16AA, 0x16BE, 0x16EB,
            0x16A9, 0x16BE, 0x16DE, 0x16EB, 0x16BB, 0x16C1, 0x16CF, 0x16EB, 0x16BE, 0x16D6, 0x16EB, 0x16BB, 0x16D6, 0x16AA, 0x16B1, 0x16D7, 0x16C1, 0x16AA,
            0x16A7, 0x16EB, 0x16D7, 0x16D6, 0x16EC
        },
        {
            0x000016C1, 0x000016B3, 0x000016EB, 0x000016D7, 0x000016A8, 0x000016B7, 0x000016EB, 0x000016B7, 0x000016DA, 0x000016A8, 0x000016CB, 0x000016EB,
            0x000016D6, 0x000016A9, 0x000016CF, 0x000016AA, 0x000016BE, 0x000016EB, 0x000016A9, 0x000016BE, 0x000016DE, 0x000016EB, 0x000016BB, 0x000016C1,
            0x000016CF, 0x000016EB, 0x000016BE, 0x000016D6, 0x000016EB, 0x000016BB, 0x000016D6, 0x000016AA, 0x000016B1, 0x000016D7, 0x000016C1, 0x000016AA,
            0x000016A7, 0x000016EB, 0x000016D7, 0x000016D6, 0x000016EC
        },
        L"ᛁᚳ᛫ᛗᚨᚷ᛫ᚷᛚᚨᛋ᛫ᛖᚩᛏᚪᚾ᛫ᚩᚾᛞ᛫ᚻᛁᛏ᛫ᚾᛖ᛫ᚻᛖᚪᚱᛗᛁᚪᚧ᛫ᛗᛖ᛬"
    },

    {
        {
            '\xE1', '\x9A', '\x9B', '\xE1', '\x9A', '\x9B', '\xE1', '\x9A', '\x89', '\xE1', '\x9A', '\x91', '\xE1', '\x9A', '\x85', '\xE1', '\x9A', '\x94',
            '\xE1', '\x9A', '\x89', '\xE1', '\x9A', '\x89', '\xE1', '\x9A', '\x94', '\xE1', '\x9A', '\x8B', '\xE1', '\x9A', '\x80', '\xE1', '\x9A', '\x94',
            '\xE1', '\x9A', '\x88', '\xE1', '\x9A', '\x94', '\xE1', '\x9A', '\x80', '\xE1', '\x9A', '\x8D', '\xE1', '\x9A', '\x82', '\xE1', '\x9A', '\x90',
            '\xE1', '\x9A', '\x85', '\xE1', '\x9A', '\x91', '\xE1', '\x9A', '\x80', '\xE1', '\x9A', '\x85', '\xE1', '\x9A', '\x94', '\xE1', '\x9A', '\x8B',
            '\xE1', '\x9A', '\x8C', '\xE1', '\x9A', '\x93', '\xE1', '\x9A', '\x85', '\xE1', '\x9A', '\x90', '\xE1', '\x9A', '\x9C'
        },
        {
            0x169B, 0x169B, 0x1689, 0x1691, 0x1685, 0x1694, 0x1689, 0x1689, 0x1694, 0x168B, 0x1680, 0x1694, 0x1688, 0x1694, 0x1680, 0x168D, 0x1682, 0x1690,
            0x1685, 0x1691, 0x1680, 0x1685, 0x1694, 0x168B, 0x168C, 0x1693, 0x1685, 0x1690, 0x169C
        },
        {
            0x0000169B, 0x0000169B, 0x00001689, 0x00001691, 0x00001685, 0x00001694, 0x00001689, 0x00001689, 0x00001694, 0x0000168B, 0x00001680, 0x00001694,
            0x00001688, 0x00001694, 0x00001680, 0x0000168D, 0x00001682, 0x00001690, 0x00001685, 0x00001691, 0x00001680, 0x00001685, 0x00001694, 0x0000168B,
            0x0000168C, 0x00001693, 0x00001685, 0x00001690, 0x0000169C
        },
        L"᚛᚛ᚉᚑᚅᚔᚉᚉᚔᚋ ᚔᚈᚔ ᚍᚂᚐᚅᚑ ᚅᚔᚋᚌᚓᚅᚐ᚜"
    },

    {
        {
            '\xE2', '\xA0', '\x8A', '\xE2', '\xA0', '\x80', '\xE2', '\xA0', '\x89', '\xE2', '\xA0', '\x81', '\xE2', '\xA0', '\x9D', '\xE2', '\xA0', '\x80',
            '\xE2', '\xA0', '\x91', '\xE2', '\xA0', '\x81', '\xE2', '\xA0', '\x9E', '\xE2', '\xA0', '\x80', '\xE2', '\xA0', '\x9B', '\xE2', '\xA0', '\x87',
            '\xE2', '\xA0', '\x81', '\xE2', '\xA0', '\x8E', '\xE2', '\xA0', '\x8E', '\xE2', '\xA0', '\x80', '\xE2', '\xA0', '\x81', '\xE2', '\xA0', '\x9D',
            '\xE2', '\xA0', '\x99', '\xE2', '\xA0', '\x80', '\xE2', '\xA0', '\x8A', '\xE2', '\xA0', '\x9E', '\xE2', '\xA0', '\x80', '\xE2', '\xA0', '\x99',
            '\xE2', '\xA0', '\x95', '\xE2', '\xA0', '\x91', '\xE2', '\xA0', '\x8E', '\xE2', '\xA0', '\x9D', '\xE2', '\xA0', '\x9E', '\xE2', '\xA0', '\x80',
            '\xE2', '\xA0', '\x93', '\xE2', '\xA0', '\xA5', '\xE2', '\xA0', '\x97', '\xE2', '\xA0', '\x9E', '\xE2', '\xA0', '\x80', '\xE2', '\xA0', '\x8D',
            '\xE2', '\xA0', '\x91'
        },
        {
            0x280A, 0x2800, 0x2809, 0x2801, 0x281D, 0x2800, 0x2811, 0x2801, 0x281E, 0x2800, 0x281B, 0x2807, 0x2801, 0x280E, 0x280E, 0x2800, 0x2801, 0x281D,
            0x2819, 0x2800, 0x280A, 0x281E, 0x2800, 0x2819, 0x2815, 0x2811, 0x280E, 0x281D, 0x281E, 0x2800, 0x2813, 0x2825, 0x2817, 0x281E, 0x2800, 0x280D,
            0x2811
        },
        {
            0x0000280A, 0x00002800, 0x00002809, 0x00002801, 0x0000281D, 0x00002800, 0x00002811, 0x00002801, 0x0000281E, 0x00002800, 0x0000281B, 0x00002807,
            0x00002801, 0x0000280E, 0x0000280E, 0x00002800, 0x00002801, 0x0000281D, 0x00002819, 0x00002800, 0x0000280A, 0x0000281E, 0x00002800, 0x00002819,
            0x00002815, 0x00002811, 0x0000280E, 0x0000281D, 0x0000281E, 0x00002800, 0x00002813, 0x00002825, 0x00002817, 0x0000281E, 0x00002800, 0x0000280D,
            0x00002811
        },
        L"⠊⠀⠉⠁⠝⠀⠑⠁⠞⠀⠛⠇⠁⠎⠎⠀⠁⠝⠙⠀⠊⠞⠀⠙⠕⠑⠎⠝⠞⠀⠓⠥⠗⠞⠀⠍⠑"
    },

    {
        {
            '\xD8', '\xA3', '\xD9', '\x86', '\xD8', '\xA7', '\x20', '\xD9', '\x82', '\xD8', '\xA7', '\xD8', '\xAF', '\xD8', '\xB1', '\x20', '\xD8', '\xB9',
            '\xD9', '\x84', '\xD9', '\x89', '\x20', '\xD8', '\xA3', '\xD9', '\x83', '\xD9', '\x84', '\x20', '\xD8', '\xA7', '\xD9', '\x84', '\xD8', '\xB2',
            '\xD8', '\xAC', '\xD8', '\xA7', '\xD8', '\xAC', '\x20', '\xD9', '\x88', '\x20', '\xD9', '\x87', '\xD8', '\xB0', '\xD8', '\xA7', '\x20', '\xD9',
            '\x84', '\xD8', '\xA7', '\x20', '\xD9', '\x8A', '\xD8', '\xA4', '\xD9', '\x84', '\xD9', '\x85', '\xD9', '\x86', '\xD9', '\x8A', '\x2E'
        },
        {
            0x0623, 0x0646, 0x0627, 0x0020, 0x0642, 0x0627, 0x062F, 0x0631, 0x0020, 0x0639, 0x0644, 0x0649, 0x0020, 0x0623, 0x0643, 0x0644, 0x0020, 0x0627,
            0x0644, 0x0632, 0x062C, 0x0627, 0x062C, 0x0020, 0x0648, 0x0020, 0x0647, 0x0630, 0x0627, 0x0020, 0x0644, 0x0627, 0x0020, 0x064A, 0x0624, 0x0644,
            0x0645, 0x0646, 0x064A, 0x002E
        },
        {
            0x00000623, 0x00000646, 0x00000627, 0x00000020, 0x00000642, 0x00000627, 0x0000062F, 0x00000631, 0x00000020, 0x00000639, 0x00000644, 0x00000649,
            0x00000020, 0x00000623, 0x00000643, 0x00000644, 0x00000020, 0x00000627, 0x00000644, 0x00000632, 0x0000062C, 0x00000627, 0x0000062C, 0x00000020,
            0x00000648, 0x00000020, 0x00000647, 0x00000630, 0x00000627, 0x00000020, 0x00000644, 0x00000627, 0x00000020, 0x0000064A, 0x00000624, 0x00000644,
            0x00000645, 0x00000646, 0x0000064A, 0x0000002E
        },
        L"أنا قادر على أكل الزجاج و هذا لا يؤلمني."
    },

    {
        {
            '\xE1', '\x80', '\x80', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9A', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9D', '\xE1', '\x80', '\x94',
            '\xE1', '\x80', '\xB9', '\xE2', '\x80', '\x8C', '\xE1', '\x80', '\x90', '\xE1', '\x80', '\xB1', '\xE1', '\x80', '\xAC', '\xE1', '\x80', '\xB9',
            '\xE2', '\x80', '\x8C', '\xE1', '\x81', '\x8A', '\xE1', '\x80', '\x80', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9A', '\xE1', '\x80', '\xB9',
            '\xE1', '\x80', '\x9D', '\xE1', '\x80', '\x94', '\xE1', '\x80', '\xB9', '\xE2', '\x80', '\x8C', '\xE1', '\x80', '\x99', '\x20', '\xE1', '\x80',
            '\x99', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9A', '\xE1', '\x80', '\x80', '\xE1', '\x80', '\xB9', '\xE2', '\x80', '\x8C', '\xE1', '\x80',
            '\x85', '\xE1', '\x80', '\xAC', '\xE1', '\x80', '\xB8', '\xE1', '\x80', '\x94', '\xE1', '\x80', '\xAF', '\xE1', '\x80', '\xAD', '\xE1', '\x80',
            '\x84', '\xE1', '\x80', '\xB9', '\xE2', '\x80', '\x8C', '\xE1', '\x80', '\x9E', '\xE1', '\x80', '\x8A', '\xE1', '\x80', '\xB9', '\xE2', '\x80',
            '\x8C', '\xE1', '\x81', '\x8B', '\x20', '\xE1', '\x81', '\x8E', '\xE1', '\x80', '\x80', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9B', '\xE1',
            '\x80', '\xB1', '\xE1', '\x80', '\xAC', '\xE1', '\x80', '\x84', '\xE1', '\x80', '\xB9', '\xE2', '\x80', '\x8C', '\xE1', '\x80', '\xB7', '\x20',
            '\xE1', '\x80', '\x91', '\xE1', '\x80', '\xAD', '\xE1', '\x80', '\x81', '\xE1', '\x80', '\xAF', '\xE1', '\x80', '\xAD', '\xE1', '\x80', '\x80',
            '\xE1', '\x80', '\xB9', '\xE2', '\x80', '\x8C', '\xE1', '\x80', '\x99', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9F', '\xE1', '\x80', '\xAF',
            '\x20', '\xE1', '\x80', '\x99', '\xE1', '\x80', '\x9B', '\xE1', '\x80', '\xB9', '\xE1', '\x80', '\x9F', '\xE1', '\x80', '\xAD', '\xE1', '\x80',
            '\x95', '\xE1', '\x80', '\xAC', '\xE1', '\x81', '\x8B'
        },
        {
            0x1000, 0x1039, 0x101A, 0x1039, 0x101D, 0x1014, 0x1039, 0x200C, 0x1010, 0x1031, 0x102C, 0x1039, 0x200C, 0x104A, 0x1000, 0x1039, 0x101A, 0x1039,
            0x101D, 0x1014, 0x1039, 0x200C, 0x1019, 0x0020, 0x1019, 0x1039, 0x101A, 0x1000, 0x1039, 0x200C, 0x1005, 0x102C, 0x1038, 0x1014, 0x102F, 0x102D,
            0x1004, 0x1039, 0x200C, 0x101E, 0x100A, 0x1039, 0x200C, 0x104B, 0x0020, 0x104E, 0x1000, 0x1039, 0x101B, 0x1031, 0x102C, 0x1004, 0x1039, 0x200C,
            0x1037, 0x0020, 0x1011, 0x102D, 0x1001, 0x102F, 0x102D, 0x1000, 0x1039, 0x200C, 0x1019, 0x1039, 0x101F, 0x102F, 0x0020, 0x1019, 0x101B, 0x1039,
            0x101F, 0x102D, 0x1015, 0x102C, 0x104B
        },
        {
            0x00001000, 0x00001039, 0x0000101A, 0x00001039, 0x0000101D, 0x00001014, 0x00001039, 0x0000200C, 0x00001010, 0x00001031, 0x0000102C, 0x00001039,
            0x0000200C, 0x0000104A, 0x00001000, 0x00001039, 0x0000101A, 0x00001039, 0x0000101D, 0x00001014, 0x00001039, 0x0000200C, 0x00001019, 0x00000020,
            0x00001019, 0x00001039, 0x0000101A, 0x00001000, 0x00001039, 0x0000200C, 0x00001005, 0x0000102C, 0x00001038, 0x00001014, 0x0000102F, 0x0000102D,
            0x00001004, 0x00001039, 0x0000200C, 0x0000101E, 0x0000100A, 0x00001039, 0x0000200C, 0x0000104B, 0x00000020, 0x0000104E, 0x00001000, 0x00001039,
            0x0000101B, 0x00001031, 0x0000102C, 0x00001004, 0x00001039, 0x0000200C, 0x00001037, 0x00000020, 0x00001011, 0x0000102D, 0x00001001, 0x0000102F,
            0x0000102D, 0x00001000, 0x00001039, 0x0000200C, 0x00001019, 0x00001039, 0x0000101F, 0x0000102F, 0x00000020, 0x00001019, 0x0000101B, 0x00001039,
            0x0000101F, 0x0000102D, 0x00001015, 0x0000102C, 0x0000104B
        },
        L"က္ယ္ဝန္‌တော္‌၊က္ယ္ဝန္‌မ မ္ယက္‌စားနုိင္‌သည္‌။ ၎က္ရောင္‌့ ထိခုိက္‌မ္ဟု မရ္ဟိပာ။"
    },

    {
        {
            '\xF0', '\x9F', '\xA0', '\x80', '\xF0', '\x9F', '\xA0', '\x81', '\xF0', '\x9F', '\xA0', '\x82', '\xF0', '\x9F', '\xA0', '\x83', '\xF0', '\x9F',
            '\xA0', '\x84', '\xF0', '\x9F', '\xA0', '\x85', '\xF0', '\x9F', '\xA0', '\x86', '\xF0', '\x9F', '\xA0', '\x87', '\xF0', '\x9F', '\xA0', '\x88',
            '\xF0', '\x9F', '\xA0', '\x89', '\xF0', '\x9F', '\xA0', '\x8A', '\xF0', '\x9F', '\xA0', '\x8B'
        },
        {
            0xD83E, 0xDC00, 0xD83E, 0xDC01, 0xD83E, 0xDC02, 0xD83E, 0xDC03, 0xD83E, 0xDC04, 0xD83E, 0xDC05, 0xD83E, 0xDC06, 0xD83E, 0xDC07, 0xD83E, 0xDC08,
            0xD83E, 0xDC09, 0xD83E, 0xDC0A, 0xD83E, 0xDC0B
        },
        {
            0x0001F800, 0x0001F801, 0x0001F802, 0x0001F803, 0x0001F804, 0x0001F805, 0x0001F806, 0x0001F807, 0x0001F808, 0x0001F809, 0x0001F80A, 0x0001F80B
        },
        L"🠀🠁🠂🠃🠄🠅🠆🠇🠈🠉🠊🠋"
    },

    {
        {
            '\xF0', '\x9F', '\x80', '\x80', '\xF0', '\x9F', '\x80', '\x81', '\xF0', '\x9F', '\x80', '\x82', '\xF0', '\x9F', '\x80', '\x83', '\xF0', '\x9F',
            '\x80', '\x84', '\xF0', '\x9F', '\x80', '\x85', '\xF0', '\x9F', '\x80', '\x86', '\xF0', '\x9F', '\x80', '\x87', '\xF0', '\x9F', '\x80', '\x88',
            '\xF0', '\x9F', '\x80', '\x89', '\xF0', '\x9F', '\x80', '\x8A', '\xF0', '\x9F', '\x80', '\x8B', '\xF0', '\x9F', '\x80', '\x8C', '\xF0', '\x9F',
            '\x80', '\x8D', '\xF0', '\x9F', '\x80', '\x8E', '\xF0', '\x9F', '\x80', '\x8F', '\xF0', '\x9F', '\x80', '\x90', '\xF0', '\x9F', '\x80', '\x91',
            '\xF0', '\x9F', '\x80', '\x92', '\xF0', '\x9F', '\x80', '\x93', '\xF0', '\x9F', '\x80', '\x94', '\xF0', '\x9F', '\x80', '\x95', '\xF0', '\x9F',
            '\x80', '\x96', '\xF0', '\x9F', '\x80', '\x97', '\xF0', '\x9F', '\x80', '\x98', '\xF0', '\x9F', '\x80', '\x99', '\xF0', '\x9F', '\x80', '\x9A',
            '\xF0', '\x9F', '\x80', '\x9B', '\xF0', '\x9F', '\x80', '\x9C', '\xF0', '\x9F', '\x80', '\x9D', '\xF0', '\x9F', '\x80', '\x9E', '\xF0', '\x9F',
            '\x80', '\x9F', '\xF0', '\x9F', '\x80', '\xA0', '\xF0', '\x9F', '\x80', '\xA1', '\xF0', '\x9F', '\x80', '\xA2', '\xF0', '\x9F', '\x80', '\xA3',
            '\xF0', '\x9F', '\x80', '\xA4', '\xF0', '\x9F', '\x80', '\xA5', '\xF0', '\x9F', '\x80', '\xA6', '\xF0', '\x9F', '\x80', '\xA7', '\xF0', '\x9F',
            '\x80', '\xA8', '\xF0', '\x9F', '\x80', '\xA9', '\xF0', '\x9F', '\x80', '\xAA', '\xF0', '\x9F', '\x80', '\xAB'
        },
        {
            0xD83C, 0xDC00, 0xD83C, 0xDC01, 0xD83C, 0xDC02, 0xD83C, 0xDC03, 0xD83C, 0xDC04, 0xD83C, 0xDC05, 0xD83C, 0xDC06, 0xD83C, 0xDC07, 0xD83C, 0xDC08,
            0xD83C, 0xDC09, 0xD83C, 0xDC0A, 0xD83C, 0xDC0B, 0xD83C, 0xDC0C, 0xD83C, 0xDC0D, 0xD83C, 0xDC0E, 0xD83C, 0xDC0F, 0xD83C, 0xDC10, 0xD83C, 0xDC11,
            0xD83C, 0xDC12, 0xD83C, 0xDC13, 0xD83C, 0xDC14, 0xD83C, 0xDC15, 0xD83C, 0xDC16, 0xD83C, 0xDC17, 0xD83C, 0xDC18, 0xD83C, 0xDC19, 0xD83C, 0xDC1A,
            0xD83C, 0xDC1B, 0xD83C, 0xDC1C, 0xD83C, 0xDC1D, 0xD83C, 0xDC1E, 0xD83C, 0xDC1F, 0xD83C, 0xDC20, 0xD83C, 0xDC21, 0xD83C, 0xDC22, 0xD83C, 0xDC23,
            0xD83C, 0xDC24, 0xD83C, 0xDC25, 0xD83C, 0xDC26, 0xD83C, 0xDC27, 0xD83C, 0xDC28, 0xD83C, 0xDC29, 0xD83C, 0xDC2A, 0xD83C, 0xDC2B
        },
        {
            0x0001F000, 0x0001F001, 0x0001F002, 0x0001F003, 0x0001F004, 0x0001F005, 0x0001F006, 0x0001F007, 0x0001F008, 0x0001F009, 0x0001F00A, 0x0001F00B,
            0x0001F00C, 0x0001F00D, 0x0001F00E, 0x0001F00F, 0x0001F010, 0x0001F011, 0x0001F012, 0x0001F013, 0x0001F014, 0x0001F015, 0x0001F016, 0x0001F017,
            0x0001F018, 0x0001F019, 0x0001F01A, 0x0001F01B, 0x0001F01C, 0x0001F01D, 0x0001F01E, 0x0001F01F, 0x0001F020, 0x0001F021, 0x0001F022, 0x0001F023,
            0x0001F024, 0x0001F025, 0x0001F026, 0x0001F027, 0x0001F028, 0x0001F029, 0x0001F02A, 0x0001F02B
        },
        L"🀀🀁🀂🀃🀄🀅🀆🀇🀈🀉🀊🀋🀌🀍🀎🀏🀐🀑🀒🀓🀔🀕🀖🀗🀘🀙🀚🀛🀜🀝🀞🀟🀠🀡🀢🀣🀤🀥🀦🀧🀨🀩🀪🀫"
    },

    {
        { '\xED', '\x9F', '\xBF' },
        { 0xD7FF },
        { utf::min_surrogate - 1 },
        L"\U0000D7FF"
    },

    {
        { '\xEE', '\x80', '\x80' },
        { 0xE000 },
        { utf::max_surrogate + 1 },
        L"\U0000E000"
    },

    {
        { '\xF0', '\x90', '\x80', '\x80' },
        { 0xD800, 0xDC00 },
        { 0x00010000 },
        L"\U00010000"
    },

    {
        { '\xF4', '\x8F', '\xBF', '\xBF' },
        { 0xDBFF, 0xDFFF },
        { utf::max_unicode_code_point },
        L"\U0010FFFF"
    },
};

supported_tuple const supported_test_data[] =
{
    {
        { '\xFA', '\x95', '\xA9', '\xB6', '\x83' },
        { 0x02569D83 }
    },

    {
        { '\xFD', '\x95', '\xA9', '\xB6', '\x83', '\xAC' },
        { 0x55A760EC }
    },
};

template<
    typename Ch>
struct utf_namer {};

template<> struct utf_namer<         char> { static char const value[]; };
template<> struct utf_namer<unsigned char> : utf_namer<char> {};
template<> struct utf_namer<signed   char> : utf_namer<char> {};
template<> struct utf_namer<char16_t     > { static char const value[]; };
template<> struct utf_namer<char32_t     > { static char const value[]; };
template<> struct utf_namer<wchar_t      > { static char const value[]; };

char const utf_namer<char    >::value[] = "UTF8";
char const utf_namer<char16_t>::value[] = "UTF16";
char const utf_namer<char32_t>::value[] = "UTF32";
char const utf_namer<wchar_t >::value[] = "UTFW";

template<
    typename Ch,
    typename Och>
void run_conv_test(
    std::basic_string<Ch> const & buf,
    std::basic_string<Och> const & obuf)
{
    typedef utf::utf_selector_t<Ch> utf_type;
    typedef utf::utf_selector_t<Och> outf_type;

    std::basic_string<Och> buf_tmp0;
    utf::convz<utf_type, outf_type>(buf.data(), std::back_inserter(buf_tmp0));
    std::basic_string<Och> buf_tmp1;
    utf::conv<utf_type, outf_type>(buf.cbegin(), buf.cend(), std::back_inserter(buf_tmp1));
    auto const success =
        obuf == buf_tmp0 &&
        obuf == buf_tmp1;
    BOOST_TEST_REQUIRE(success);
}

template<
    typename Ch>
void run_size_test(
    std::basic_string<Ch> const & buf)
{
    typedef utf::utf_selector_t<Ch> utf_type;

    size_t total_size0 = 0;
    for (auto str = buf.data(); *str;)
    {
        auto const size = utf::sizech<utf_type>(str);
        BOOST_TEST_REQUIRE(!!size);
        total_size0 += size;
        str += size;
    }
    BOOST_TEST_REQUIRE(buf.size() == total_size0);

    auto const total_size1 = utf::sizez<utf_type>(buf.data());
    BOOST_TEST_REQUIRE(buf.size() == total_size1);

    auto const total_size2 = utf::size<utf_type>(buf.cbegin(), buf.cend());
    BOOST_TEST_REQUIRE(buf.size() == total_size2);
}

}

BOOST_DATA_TEST_CASE(conv_u8_to_u8  , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u8 , tuple.u8 ); }
BOOST_DATA_TEST_CASE(conv_u8_to_u16 , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u8 , tuple.u16); }
BOOST_DATA_TEST_CASE(conv_u8_to_u32 , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u8 , tuple.u32); }
BOOST_DATA_TEST_CASE(conv_u8_to_uw  , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u8 , tuple.uw ); }
BOOST_DATA_TEST_CASE(conv_u16_to_u8 , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u16, tuple.u8 ); }
BOOST_DATA_TEST_CASE(conv_u16_to_u16, boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u16, tuple.u16); }
BOOST_DATA_TEST_CASE(conv_u16_to_u32, boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u16, tuple.u32); }
BOOST_DATA_TEST_CASE(conv_u16_to_uw , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u16, tuple.uw ); }
BOOST_DATA_TEST_CASE(conv_u32_to_u8 , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u32, tuple.u8 ); }
BOOST_DATA_TEST_CASE(conv_u32_to_u16, boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u32, tuple.u16); }
BOOST_DATA_TEST_CASE(conv_u32_to_u32, boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u32, tuple.u32); }
BOOST_DATA_TEST_CASE(conv_u32_to_uw , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.u32, tuple.uw ); }
BOOST_DATA_TEST_CASE(conv_uw_to_u8  , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.uw , tuple.u8 ); }
BOOST_DATA_TEST_CASE(conv_uw_to_u16 , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.uw , tuple.u16); }
BOOST_DATA_TEST_CASE(conv_uw_to_u32 , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.uw , tuple.u32); }
BOOST_DATA_TEST_CASE(conv_uw_to_uw  , boost::make_iterator_range(unicode_test_data), tuple) { run_conv_test(tuple.uw , tuple.uw ); }

BOOST_DATA_TEST_CASE(conv_u32_to_u8_supported, boost::make_iterator_range(supported_test_data), tuple) { run_conv_test(tuple.u32, tuple.u8 ); }
BOOST_DATA_TEST_CASE(conv_u8_to_u32_supported, boost::make_iterator_range(supported_test_data), tuple) { run_conv_test(tuple.u8 , tuple.u32); }

BOOST_DATA_TEST_CASE(size_u8 , boost::make_iterator_range(unicode_test_data), tuple) { run_size_test(tuple.u8 ); }
BOOST_DATA_TEST_CASE(size_u16, boost::make_iterator_range(unicode_test_data), tuple) { run_size_test(tuple.u16); }
BOOST_DATA_TEST_CASE(size_u32, boost::make_iterator_range(unicode_test_data), tuple) { run_size_test(tuple.u32); }
BOOST_DATA_TEST_CASE(size_uw , boost::make_iterator_range(unicode_test_data), tuple) { run_size_test(tuple.uw ); }

BOOST_DATA_TEST_CASE(size_u8_supported , boost::make_iterator_range(supported_test_data), tuple) { run_size_test(tuple.u8 ); }
BOOST_DATA_TEST_CASE(size_u32_supported, boost::make_iterator_range(supported_test_data), tuple) { run_size_test(tuple.u32); }

BOOST_STATIC_ASSERT(utf::is_utf_same<unsigned char,          char>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<unsigned char, unsigned char>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<unsigned char, signed   char>::value);

BOOST_STATIC_ASSERT(utf::is_utf_same<signed char,          char>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<signed char, unsigned char>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<signed char, signed   char>::value);

BOOST_STATIC_ASSERT(utf::is_utf_same<char,          char>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<char, unsigned char>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<char, signed   char>::value);

BOOST_STATIC_ASSERT(utf::is_utf_same<char16_t, char16_t>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<char32_t, char32_t>::value);
BOOST_STATIC_ASSERT(utf::is_utf_same<wchar_t , wchar_t >::value);

BOOST_STATIC_ASSERT(!utf::is_utf_same<char, char16_t>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<char, char32_t>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<char, wchar_t >::value);

BOOST_STATIC_ASSERT(!utf::is_utf_same<char16_t, char>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<char32_t, char>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<wchar_t , char>::value);

BOOST_STATIC_ASSERT(!utf::is_utf_same<char16_t, char32_t>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<char32_t, char16_t>::value);

#if defined(_WIN32)

BOOST_STATIC_ASSERT( utf::is_utf_same<char16_t, wchar_t>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<char32_t, wchar_t>::value);

BOOST_STATIC_ASSERT( utf::is_utf_same<wchar_t, char16_t>::value);
BOOST_STATIC_ASSERT(!utf::is_utf_same<wchar_t, char32_t>::value);

#elif defined(__linux__) || defined(__APPLE__)

BOOST_STATIC_ASSERT(!utf::is_utf_same<char16_t, wchar_t>::value);
BOOST_STATIC_ASSERT( utf::is_utf_same<char32_t, wchar_t>::value);

BOOST_STATIC_ASSERT(!utf::is_utf_same<wchar_t, char16_t>::value);
BOOST_STATIC_ASSERT( utf::is_utf_same<wchar_t, char32_t>::value);

#else
    #error Unknown platform
#endif

#if __cpp_constexpr >= 201603

BOOST_STATIC_ASSERT(utf::is_utf_same_v<unsigned char,          char>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<unsigned char, unsigned char>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<unsigned char, signed   char>);

BOOST_STATIC_ASSERT(utf::is_utf_same_v<signed char,          char>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<signed char, unsigned char>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<signed char, signed   char>);

BOOST_STATIC_ASSERT(utf::is_utf_same_v<char,          char>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<char, unsigned char>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<char, signed   char>);

BOOST_STATIC_ASSERT(utf::is_utf_same_v<char16_t, char16_t>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<char32_t, char32_t>);
BOOST_STATIC_ASSERT(utf::is_utf_same_v<wchar_t , wchar_t >);

BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char, char16_t>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char, char32_t>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char, wchar_t >);

BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char16_t, char>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char32_t, char>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<wchar_t , char>);

BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char16_t, char32_t>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char32_t, char16_t>);

#if defined(_WIN32)

BOOST_STATIC_ASSERT( utf::is_utf_same_v<char16_t, wchar_t>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char32_t, wchar_t>);

BOOST_STATIC_ASSERT( utf::is_utf_same_v<wchar_t, char16_t>);
BOOST_STATIC_ASSERT(!utf::is_utf_same_v<wchar_t, char32_t>);

#elif defined(__linux__) || defined(__APPLE__)

BOOST_STATIC_ASSERT(!utf::is_utf_same_v<char16_t, wchar_t>);
BOOST_STATIC_ASSERT( utf::is_utf_same_v<char32_t, wchar_t>);

BOOST_STATIC_ASSERT(!utf::is_utf_same_v<wchar_t, char16_t>);
BOOST_STATIC_ASSERT( utf::is_utf_same_v<wchar_t, char32_t>);

#else
    #error Unknown platform
#endif

#endif

namespace {

uint64_t get_time() throw()
{
#if defined(_MSC_VER)

#if defined(_M_IX86) || defined(_M_X64)
    // Note: CPUID can be executed at any privilege level to serialize instruction execution. Serializing instruction
    //       execution guarantees that any modifications to flags, registers, and memory for previous instructions are
    //       completed before the next instruction is fetched and executed.
    int cpu_info[4];
    __cpuid(cpu_info, 0);
    return __rdtsc();
#else
    #error Unsupported architecture
#endif

#elif defined(__GNUC__) || defined(__clang__)

#if defined(__i386__) || defined(__x86_64__)
    uint32_t lo, hi;
    asm volatile(
        "cpuid\n\t"
        "rdtsc\n\t"
        : "=a"(lo), "=d"(hi) : "a"(0) :
#if defined(__i386__)
        "%ebx", "%ecx"
#else
        "%rbx", "%rcx"
#endif
        );
    return static_cast<uint64_t>(hi) << 32 | lo;
#else
    #error Unsupported architecture
#endif

#else
    #error Unknown compiler
#endif
}

#if defined(__linux__) || defined(__APPLE__)
void current_utc_time(timespec * ts)
{
#if defined(__MACH__) // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    if (host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock) != KERN_SUCCESS)
        std::abort();
    if (clock_get_time(cclock, &mts) != KERN_SUCCESS)
        std::abort();
    if (mach_port_deallocate(mach_task_self(), cclock) != KERN_SUCCESS)
        std::abort();
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
#else
    if (clock_gettime(CLOCK_REALTIME, ts))
        std::abort();
#endif
}
#endif

uint64_t get_time_resolution() throw()
{
#if defined(_WIN32)
    LARGE_INTEGER beg_orig, end_orig, freq_orig;
    if (!QueryPerformanceCounter(&beg_orig))
        std::abort();
    auto const beg_time = get_time();
    Sleep(1000);
    if (!QueryPerformanceCounter(&end_orig))
        std::abort();
    auto const end_time = get_time();
    if (!QueryPerformanceFrequency(&freq_orig))
        std::abort();
    auto const elapsed_orig = end_orig.QuadPart - beg_orig.QuadPart;
    auto const elapsed_time = end_time - beg_time;
    return (freq_orig.QuadPart * elapsed_time + elapsed_orig / 2) / elapsed_orig;
#elif defined(__linux__) || defined(__APPLE__)
    timespec beg_orig, end_orig;
    current_utc_time(&beg_orig);
    auto const beg_time = get_time();
    sleep(1);
    current_utc_time(&end_orig);
    auto const end_time = get_time();
    std::chrono::nanoseconds const elapsed_orig =
        (std::chrono::seconds(end_orig.tv_sec) + std::chrono::nanoseconds(end_orig.tv_nsec)) -
        (std::chrono::seconds(beg_orig.tv_sec) + std::chrono::nanoseconds(beg_orig.tv_nsec));
    auto const elapsed_time = end_time - beg_time;
    return static_cast<uint64_t>(elapsed_time * 1000000000ull / elapsed_orig.count());
#else
    #error Unknown platform
#endif
}

size_t const warmup_iterations = 4;
size_t const measure_iterations = 64;

template<
    typename MeasureFn>
double measure(
    uint64_t const resolution,
    MeasureFn && measure_fn)
{
    uint64_t sum_duration = 0;
    for (size_t n = 0; n < warmup_iterations + measure_iterations; ++n)
    {
        uint64_t const beg_time = get_time();
        measure_fn();
        uint64_t const end_time = get_time();
        if (n >= warmup_iterations)
            sum_duration += end_time - beg_time;
    }
    return static_cast<double>(sum_duration) / measure_iterations / resolution;
}

template<
    typename Ch,
    typename Och>
void dump_name()
{
    static size_t const name_width = 5;
    std::cout <<
        std::left << std::setw(name_width) << utf_namer<Ch>::value  << " ==> " <<
        std::left << std::setw(name_width) << utf_namer<Och>::value << ": ";
}

void dump_duration(double const duration)
{
    std::cout << std::fixed << std::setprecision(9) << duration << "s";
}


void dump_difference(double const duration, double const base_duration)
{
    auto const percents = 100 * (duration / base_duration - 1);
    std::cout << " (" << (percents > 0 ? "+" : "") << std::fixed << std::setprecision(2) << percents << "%)";
}

void dump_endl()
{
    std::cout << std::endl;
}

template<
    typename Ch,
    typename Och>
double run_measure(
    uint64_t const resolution,
    std::vector<Ch> const & buf,
    std::vector<Och> const & obuf)
{
    typedef utf::utf_selector_t<Ch> utf_type;
    typedef utf::utf_selector_t<Och> outf_type;

    std::vector<Och> res;
    res.reserve(obuf.capacity());
    auto const duration = measure(resolution, [&]
        {
            res.clear();
            utf::conv<utf_type, outf_type>(&buf.front(), &buf.back() + 1, std::back_inserter(res));
        });
    BOOST_TEST_REQUIRE(res.size() == obuf.size());
    auto const same = memcmp(&obuf.front(), &res.front(), sizeof(Och) * res.size()) == 0;
    BOOST_TEST_REQUIRE(same);

    dump_name<Ch, Och>();
    dump_duration(duration);
    dump_endl();
    return duration;
}

}

#if defined(WW898_ENABLE_PERFORMANCE_TESTS)
#define WW898_PERFORMANCE_TESTS_MODE *boost::unit_test::enabled()
#else
#define WW898_PERFORMANCE_TESTS_MODE *boost::unit_test::disabled()
#endif

BOOST_AUTO_TEST_CASE(performance, WW898_PERFORMANCE_TESTS_MODE)
{
    static size_t const symbol_count = 16 * 1024 * 1024;

    std::vector<char    > buf_u8 ;
    std::vector<char16_t> buf_u16;
    std::vector<char32_t> buf_u32;
    std::vector<wchar_t > buf_uw ;

    {
        boost::random::mt19937 random(0);
        for (auto n = symbol_count; n-- > 0; )
        {
            auto cp = random() % (utf::max_unicode_code_point + 1);
            if (utf::is_surrogate(cp))
                cp -= utf::min_surrogate;

#if defined(__GNUC__)
            // Bug: std::codecvt_utf8_utf16 for GCC v5.4.0 for Ubuntu 16.04 LTS failed to convert \xFFFF code point correctly.
            if (cp == 0xFFFF)
                cp = 0xFEFF;
#endif

            buf_u32.push_back(cp);
        }
        utf::conv<utf::utf32, utf::utf8 >(buf_u32.cbegin(), buf_u32.cend(), std::back_inserter(buf_u8 ));
        utf::conv<utf::utf32, utf::utf16>(buf_u32.cbegin(), buf_u32.cend(), std::back_inserter(buf_u16));
        utf::conv<utf::utf32, utf::utfw >(buf_u32.cbegin(), buf_u32.cend(), std::back_inserter(buf_uw ));
    }

    std::cout <<
        "__cpp_lib_string_view: " << std::dec << __cpp_lib_string_view << std::endl <<
        "__cpp_constexpr      : " << std::dec << __cpp_constexpr << std::endl <<
        "sizeof wchar_t: " << sizeof(wchar_t) << std::endl <<
        utf_namer<wchar_t >::value << ": UTF" << 8 * sizeof(wchar_t) << std::endl;

    auto const resolution = get_time_resolution();
    std::cout << "Resolution: " << resolution << std::endl;

                                 run_measure(resolution, buf_u8 , buf_u8 );
    auto const u8_u16_duration = run_measure(resolution, buf_u8 , buf_u16);
                                 run_measure(resolution, buf_u8 , buf_u32);
    auto const u8_uw_duration  = run_measure(resolution, buf_u8 , buf_uw );
    auto const u16_u8_duration = run_measure(resolution, buf_u16, buf_u8 );
                                 run_measure(resolution, buf_u16, buf_u16);
                                 run_measure(resolution, buf_u16, buf_u32);
                                 run_measure(resolution, buf_u16, buf_uw );
                                 run_measure(resolution, buf_u32, buf_u8 );
                                 run_measure(resolution, buf_u32, buf_u16);
                                 run_measure(resolution, buf_u32, buf_u32);
                                 run_measure(resolution, buf_u32, buf_uw );
    auto const uw_u8_duration  = run_measure(resolution, buf_uw , buf_u8 );
                                 run_measure(resolution, buf_uw , buf_u16);
                                 run_measure(resolution, buf_uw , buf_u32);
                                 run_measure(resolution, buf_uw , buf_uw );

    {
#if _MSC_VER >= 1900
        // Bug: MSVC 2017 linker: error LNK2001: unresolved external symbol "__declspec(dllimport) public: static class std::locale::id std::codecvt<char16_t,char,struct _Mbstatet>::id" (__imp_?id@?$codecvt@_SDU_Mbstatet@@@std@@2V0locale@2@A)
        using char16_type = int16_t;
#else
        using char16_type = char16_t;
#endif

        std::cout << "codecvt_utf8_utf16<char16_t>:" << std::endl;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_type
#if defined(__GNUC__)
            // Bug: The std::little_endian should be selected directly in std::codecvt_utf8_utf16 for GCC v5.4.0 for Ubuntu 16.04 LTS.
            , ww898::utf::max_unicode_code_point
            , std::little_endian
#endif
            >, char16_type> cvt;

        {
            std::string res;
            auto const duration = measure(resolution, [&]
                {
                    res = cvt.to_bytes(reinterpret_cast<char16_type const *>(&buf_u16.front()), reinterpret_cast<char16_type const *>(&buf_u16.back()) + 1);
                });
            BOOST_TEST_REQUIRE(res.size() == buf_u8.size());
            auto const same = memcmp(&buf_u8.front(), &res.front(), buf_u8.size()) == 0;
            BOOST_TEST_REQUIRE(same);

            dump_name<char16_t, char>();
            dump_duration(duration);
            dump_difference(duration, u16_u8_duration);
            dump_endl();
        }

        {
            std::basic_string<char16_type> res;
            auto const duration = measure(resolution, [&] { res = cvt.from_bytes(&buf_u8.front(), &buf_u8.back() + 1); });
            BOOST_TEST_REQUIRE(res.size() == buf_u16.size());
            auto const same = memcmp(&buf_u16.front(), &res.front(), sizeof(char16_t) * buf_u16.size()) == 0;
            BOOST_TEST_REQUIRE(same);

            dump_name<char, char16_t>();
            dump_duration(duration);
            dump_difference(duration, u8_u16_duration);
            dump_endl();
        }
    }

    {
#if defined(_WIN32)
        std::cout << "codecvt_utf8_utf16<wchar_t>:" << std::endl;
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cvt;
#else
        std::cout << "codecvt_utf8<wchar_t>:" << std::endl;
        std::wstring_convert<std::codecvt_utf8<wchar_t
#if defined(__GNUC__)
            // Bug: The std::little_endian should be selected directly in std::codecvt_utf8_utf16 for GCC v5.4.0 for Ubuntu 16.04 LTS.
            , ww898::utf::max_unicode_code_point
            , std::little_endian
#endif
            >, wchar_t> cvt;
#endif

        {
            std::string res;
            auto const duration = measure(resolution, [&] { res = cvt.to_bytes(&buf_uw.front(), &buf_uw.back() + 1); });
            BOOST_TEST_REQUIRE(res.size() == buf_u8.size());
            auto const same = memcmp(&buf_u8.front(), &res.front(), buf_u8.size()) == 0;
            BOOST_TEST_REQUIRE(same);

            dump_name<wchar_t, char>();
            dump_duration(duration);
            dump_difference(duration, uw_u8_duration);
            dump_endl();
        }

        {
            std::wstring res;
            auto const duration = measure(resolution, [&] { res = cvt.from_bytes(&buf_u8.front(), &buf_u8.back() + 1); });
            BOOST_TEST_REQUIRE(res.size() == buf_uw.size());
            auto const same = memcmp(&buf_uw.front(), &res.front(), sizeof(wchar_t) * buf_uw.size()) == 0;
            BOOST_TEST_REQUIRE(same);

            dump_name<char, wchar_t>();
            dump_duration(duration);
            dump_difference(duration, u8_uw_duration);
            dump_endl();
        }
    }
}

BOOST_AUTO_TEST_CASE(example, WW898_PERFORMANCE_TESTS_MODE)
{
    // यूनिकोड
    static char const u8s[] = "\xE0\xA4\xAF\xE0\xA5\x82\xE0\xA4\xA8\xE0\xA4\xBF\xE0\xA4\x95\xE0\xA5\x8B\xE0\xA4\xA1";
    using namespace ww898::utf;
    std::u16string u16;
    convz<utf_selector_t<decltype(*u8s)>, utf16>(u8s, std::back_inserter(u16));
    std::u32string u32;
    conv<utf16, utf_selector_t<decltype(u32)::value_type>>(u16.begin(), u16.end(), std::back_inserter(u32));
    std::vector<char> u8;
    convz<utf32, utf8>(u32.data(), std::back_inserter(u8));
    std::wstring uw;
    conv<utf8, utfw>(u8s, u8s + sizeof(u8s), std::back_inserter(uw));
    auto u8r = conv<char>(uw);
    auto uwr = convz<wchar_t>(u8s);

#if __cpp_lib_string_view >= 201606
    auto u32r = conv<char32_t>(std::string_view(u8r.data(), u8r.size())); // C++17 only
#endif

    static_assert(is_utf_same<decltype(*u8s), decltype(u8)::value_type>::value, "Fail");
    static_assert(
        is_utf_same<decltype(u16)::value_type, decltype(uw)::value_type>::value !=
        is_utf_same<decltype(u32)::value_type, decltype(uw)::value_type>::value, "Fail");

#if __cpp_constexpr >= 201603
    static_assert(is_utf_same_v<decltype(*u8s), decltype(u8)::value_type>, "Fail"); // C++17 only
    static_assert(
        is_utf_same_v<decltype(u16)::value_type, decltype(uw)::value_type> !=
        is_utf_same_v<decltype(u32)::value_type, decltype(uw)::value_type>, "Fail"); // C++17 only
#endif
}

#undef WW898_PERFORMANCE_TESTS_MODE

BOOST_AUTO_TEST_SUITE_END()

}}

BOOST_TEST_DONT_PRINT_LOG_VALUE(ww898::test::utf_converters::unicode_tuple)
BOOST_TEST_DONT_PRINT_LOG_VALUE(ww898::test::utf_converters::supported_tuple)
