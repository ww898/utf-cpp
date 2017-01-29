#UTF-8/16/32 C++ library
This is the C++11 template based header only library to convert UFT-8/16/32 symbols and strings. Null-terminated strings are also supported.

UTF-8 and UTF-32 (UCS-32) both support 31 bit wide code points `[0‥0x7FFFFFFF]`with no restriction. UTF-16 supports only unicode code points `[0‥0x10FFFF]` where high `[0xD800‥0xDBFF]` and low `[0xDC00‥0xDFFF]` surrogate regions are prohibited.

The maximum UTF-16 symbol size is 2 words (4 bytes, both words should be from the surrogate region). UFT-32 (UCS-32) is always 1 word (4 bytes). UTF-8 has the maximum symbol size (see [conversion table](#conversion-table)):
- 4 bytes for unicode code points
- 6 bytes for 31bit code points

##Supported compilers
Tested on following compilers:
- Visual Studio 2013 Update 5
- Visual Studio 2015 Update 3
- GCC 4.8.4
- Clang 3.4

##Usage example
```cpp
// यूनिकोड
static char const _Src[] = "\xE0\xA4\xAF\xE0\xA5\x82\xE0\xA4\xA8\xE0\xA4\xBF\xE0\xA4\x95\xE0\xA5\x8B\xE0\xA4\xA1";
using namespace ww898;
std::u16string _Utf16;
utf::convz<utf::utf8, utf::utf16>(_Src, std::back_inserter(_Utf16));
std::u32string _Utf32;
utf::conv<utf::utf16, utf::utf32>(_Utf16.begin(), _Utf16.end(), std::back_inserter(_Utf32));
std::vector<char> _Utf8;
utf::convz<utf::utf32, utf::utf8>(_Utf32.begin(), std::back_inserter(_Utf8));
```

##Conversion table
![UTF-8/32 table](https://upload.wikimedia.org/wikipedia/commons/3/38/UTF-8_Encoding_Scheme.png)
