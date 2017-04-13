# UTF-8/16/32 C++ library
This is the C++11 template based header only library to convert UFT-8/16/32 symbols and strings. Null-terminated strings are also supported.

UTF-8 and UTF-32 (UCS-32) both support 31 bit wide code points `[0‥0x7FFFFFFF]`with no restriction. UTF-16 supports only unicode code points `[0‥0x10FFFF]` where high `[0xD800‥0xDBFF]` and low `[0xDC00‥0xDFFF]` surrogate regions are prohibited.

The maximum UTF-16 symbol size is 2 words (4 bytes, both words should be from the surrogate region). UFT-32 (UCS-32) is always 1 word (4 bytes). UTF-8 has the maximum symbol size (see [conversion table](#conversion-table)):
- 4 bytes for unicode code points
- 6 bytes for 31bit code points

###### UTF-16 surrogate decoder:
|High\Low|DC00|DC01|…|DFFF|
|:-:|:-:|:-:|:-:|:-:|
|**D800**|010000|010001|…|0103FF|
|**D801**|010400|010401|…|0107FF|
|**⋮**|⋮|⋮|⋱|⋮|
|**DBFF**|10FC00|10FC01|…|10FFFF|

## Supported compilers

Tested on following compilers:
- Visual Studio 2013 Update 5
- Visual Studio 2015 Update 3
- Visual Studio 2017
- GCC 5.4.0
- Clang 3.8.0

## Usage example

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

## Performance
#### Windows x86 (Visual Studio 2013 Update 5):
```cpp
Running 225 test cases...
Resolution: 2592001699
UTF32 ==> UTF8 : 0.335336s
UTF32 ==> UTF16: 0.091804s
UTF16 ==> UTF8 : 0.433169s
UTF16 ==> UTF32: 0.097712s
UTF8  ==> UTF16: 0.217497s
UTF8  ==> UTF32: 0.140362s
codecvt_utf8_utf16<wchar_t>:
UTF16 ==> UTF8 : 0.594638s (+37.28%)
UTF8  ==> UTF16: 0.378968s (+74.24%)

*** No errors detected
```

#### Windows x64 (Visual Studio 2013 Update 5):
```cpp
Running 225 test cases...
Resolution: 2592004604
UTF32 ==> UTF8 : 0.190584s
UTF32 ==> UTF16: 0.108506s
UTF16 ==> UTF8 : 0.207430s
UTF16 ==> UTF32: 0.089672s
UTF8  ==> UTF16: 0.210476s
UTF8  ==> UTF32: 0.168318s
codecvt_utf8_utf16<wchar_t>:
UTF16 ==> UTF8 : 0.463934s (+123.66%)
UTF8  ==> UTF16: 0.318946s (+51.54%)

*** No errors detected
```

#### Windows x86 (Visual Studio 2017):
```cpp
Running 225 test cases...
Resolution: 2591999972
UTF32 ==> UTF8 : 0.273872s
UTF32 ==> UTF16: 0.149031s
UTF16 ==> UTF8 : 0.368761s
UTF16 ==> UTF32: 0.098164s
UTF8  ==> UTF16: 0.268979s
UTF8  ==> UTF32: 0.166941s
codecvt_utf8_utf16<wchar_t>:
UTF16 ==> UTF8 : 0.517618s (+40.37%)
UTF8  ==> UTF16: 0.349681s (+30.00%)

*** No errors detected
```

#### Windows x64 (Visual Studio 2017):
```cpp
Running 225 test cases...
Resolution: 2591998081
UTF32 ==> UTF8 : 0.260009s
UTF32 ==> UTF16: 0.141919s
UTF16 ==> UTF8 : 0.257103s
UTF16 ==> UTF32: 0.085505s
UTF8  ==> UTF16: 0.238545s
UTF8  ==> UTF32: 0.160184s
codecvt_utf8_utf16<wchar_t>:
UTF16 ==> UTF8 : 0.491465s (+91.15%)
UTF8  ==> UTF16: 0.311356s (+30.52%)

*** No errors detected
```

#### Ubuntu 16.04 LTS x64 (GCC 5.4.0):
```cpp
Running 225 test cases...
Resolution: 2592003903
UTF32 ==> UTF8 : 0.215397s
UTF32 ==> UTF16: 0.102215s
UTF16 ==> UTF8 : 0.214120s
UTF16 ==> UTF32: 0.052067s
UTF8  ==> UTF16: 0.187837s
UTF8  ==> UTF32: 0.143444s
codecvt_utf8_utf16<wchar_t>:
UTF16 ==> UTF8 : 0.278069s (+29.87%)
UTF8  ==> UTF16: 1.030013s (+448.36%)

*** No errors detected
```

#### Ubuntu 16.04 LTS x64 (Clang 3.8.0):
```cpp
Running 225 test cases...
Resolution: 2592004346
UTF32 ==> UTF8 : 0.169015s
UTF32 ==> UTF16: 0.097330s
UTF16 ==> UTF8 : 0.197774s
UTF16 ==> UTF32: 0.085933s
UTF8  ==> UTF16: 0.172294s
UTF8  ==> UTF32: 0.130314s
codecvt_utf8_utf16<wchar_t>:
UTF16 ==> UTF8 : 0.316627s (+60.10%)
UTF8  ==> UTF16: 0.592549s (+243.92%)

*** No errors detected
```

## Conversion table
![UTF-8/32 table](https://upload.wikimedia.org/wikipedia/commons/3/38/UTF-8_Encoding_Scheme.png)
