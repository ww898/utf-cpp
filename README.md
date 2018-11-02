# UTF-8/16/32 C++ library
This is the C++11 template based header only library under Windows/Linux/MacOs to convert UFT-8/16/32 symbols and strings. The library transparently support `wchar_t` as UTF-16 for Windows and UTF-32 for Linux and MacOs.

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

![UTF-16 Surrogates](https://upload.wikimedia.org/wikipedia/commons/thumb/b/b8/Utf-16.svg/512px-Utf-16.svg.png)

## Supported compilers

Tested on following compilers:
- Visual Studio 2013 v12.0.40629.00 Update 5
- Visual Studio 2017 v15.6.7
- GCC v5.4.0
- Clang v3.9.1/v6.0.0

## Usage example

```cpp
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
auto u32r = conv<char32_t>(std::string_view(u8r.data(), u8r.size())); // C++17 only
static_assert(is_utf_same_v<decltype(*u8s), decltype(u8)::value_type>, "Fail"); // C++17 only
static_assert(
    is_utf_same<decltype(u16)::value_type, decltype(uw)::value_type>::value !=
    is_utf_same<decltype(u32)::value_type, decltype(uw)::value_type>::value, "Fail");
```

## Performance
#### Windows x86 (Visual Studio 2013 v12.0.40629.00 Update 5):
```cpp
Running 489 test cases...
sizeof wchar_t: 2
UTFW: UTF16
Resolution: 2591998334
UTF8  ==> UTF8 : 0.163960290s
UTF8  ==> UTF16: 0.282665666s
UTF8  ==> UTF32: 0.149002153s
UTF8  ==> UTFW : 0.283254604s
UTF16 ==> UTF8 : 0.266152488s
UTF16 ==> UTF16: 0.080108020s
UTF16 ==> UTF32: 0.101033595s
UTF16 ==> UTFW : 0.094183924s
UTF32 ==> UTF8 : 0.215850861s
UTF32 ==> UTF16: 0.146806864s
UTF32 ==> UTF32: 0.042549969s
UTF32 ==> UTFW : 0.146204410s
UTFW  ==> UTF8 : 0.266856024s
UTFW  ==> UTF16: 0.094266542s
UTFW  ==> UTF32: 0.102790712s
UTFW  ==> UTFW : 0.080478961s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.685873190s (+157.70%)
UTF8  ==> UTF16: 0.466883577s (+65.17%)
codecvt_utf8_utf16<wchar_t>:
UTFW  ==> UTF8 : 0.683433984s (+156.11%)
UTF8  ==> UTFW : 0.456086023s (+61.02%)

*** No errors detected
```

#### Windows x64 (Visual Studio 2013 v12.0.40629.00 Update 5):
```cpp
Running 489 test cases...
sizeof wchar_t: 2
UTFW: UTF16
Resolution: 2591994871
UTF8  ==> UTF8 : 0.196164103s
UTF8  ==> UTF16: 0.220423499s
UTF8  ==> UTF32: 0.180234824s
UTF8  ==> UTFW : 0.217163697s
UTF16 ==> UTF8 : 0.212900399s
UTF16 ==> UTF16: 0.097028914s
UTF16 ==> UTF32: 0.101757423s
UTF16 ==> UTFW : 0.071567645s
UTF32 ==> UTF8 : 0.196917702s
UTF32 ==> UTF16: 0.112344089s
UTF32 ==> UTF32: 0.049047871s
UTF32 ==> UTFW : 0.112364705s
UTFW  ==> UTF8 : 0.211841364s
UTFW  ==> UTF16: 0.070938743s
UTFW  ==> UTF32: 0.102185818s
UTFW  ==> UTFW : 0.097848249s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.539077998s (+153.21%)
UTF8  ==> UTF16: 0.396618873s (+79.93%)
codecvt_utf8_utf16<wchar_t>:
UTFW  ==> UTF8 : 0.537690842s (+153.82%)
UTF8  ==> UTFW : 0.412762006s (+90.07%)

*** No errors detected
```

#### Windows x86 (Visual Studio 2017 v15.6.7):
```cpp
Running 489 test cases...
sizeof wchar_t: 2
UTFW: UTF16
Resolution: 2591998780
UTF8  ==> UTF8 : 0.225589121s
UTF8  ==> UTF16: 0.205551657s
UTF8  ==> UTF32: 0.135360995s
UTF8  ==> UTFW : 0.206828091s
UTF16 ==> UTF8 : 0.284084302s
UTF16 ==> UTF16: 0.109397058s
UTF16 ==> UTF32: 0.101644463s
UTF16 ==> UTFW : 0.131424306s
UTF32 ==> UTF8 : 0.291001165s
UTF32 ==> UTF16: 0.149109674s
UTF32 ==> UTF32: 0.062499637s
UTF32 ==> UTFW : 0.148655518s
UTFW  ==> UTF8 : 0.300835299s
UTFW  ==> UTF16: 0.127525400s
UTFW  ==> UTF32: 0.097031381s
UTFW  ==> UTFW : 0.109990072s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.552393684s (+94.45%)
UTF8  ==> UTF16: 0.404987578s (+97.02%)
codecvt_utf8_utf16<wchar_t>:
UTFW  ==> UTF8 : 0.596080263s (+98.14%)
UTF8  ==> UTFW : 0.418794256s (+102.48%)

*** No errors detected
```

#### Windows x64 (Visual Studio 2017 v15.6.7):
```cpp
Running 489 test cases...
sizeof wchar_t: 2
UTFW: UTF16
Resolution: 2592011526
UTF8  ==> UTF8 : 0.185124459s
UTF8  ==> UTF16: 0.191509469s
UTF8  ==> UTF32: 0.139597283s
UTF8  ==> UTFW : 0.198169193s
UTF16 ==> UTF8 : 0.243126679s
UTF16 ==> UTF16: 0.096481336s
UTF16 ==> UTF32: 0.088010385s
UTF16 ==> UTFW : 0.105519284s
UTF32 ==> UTF8 : 0.218815968s
UTF32 ==> UTF16: 0.114674103s
UTF32 ==> UTF32: 0.050287083s
UTF32 ==> UTFW : 0.115018940s
UTFW  ==> UTF8 : 0.242360203s
UTFW  ==> UTF16: 0.105936683s
UTFW  ==> UTF32: 0.088388864s
UTFW  ==> UTFW : 0.098212312s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.508659574s (+109.22%)
UTF8  ==> UTF16: 0.372852507s (+94.69%)
codecvt_utf8_utf16<wchar_t>:
UTFW  ==> UTF8 : 0.526355029s (+117.18%)
UTF8  ==> UTFW : 0.383913994s (+93.73%)

*** No errors detected
```

#### Ubuntu 16.04 LTS x64 (GCC v5.4.0):
```cpp
Running 489 test cases...
sizeof wchar_t: 4
UTFW: UTF32
Resolution: 3400052319
UTF8  ==> UTF8 : 0.110866077s
UTF8  ==> UTF16: 0.141338578s
UTF8  ==> UTF32: 0.081097171s
UTF8  ==> UTFW : 0.090628401s
UTF16 ==> UTF8 : 0.186256965s
UTF16 ==> UTF16: 0.058923306s
UTF16 ==> UTF32: 0.041104444s
UTF16 ==> UTFW : 0.041324722s
UTF32 ==> UTF8 : 0.166990347s
UTF32 ==> UTF16: 0.079132988s
UTF32 ==> UTF32: 0.030674187s
UTF32 ==> UTFW : 0.028661489s
UTFW  ==> UTF8 : 0.166499877s
UTFW  ==> UTF16: 0.075715211s
UTFW  ==> UTF32: 0.028246457s
UTFW  ==> UTFW : 0.031145368s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.166462098s (-10.63%)
UTF8  ==> UTF16: 0.412099566s (+191.57%)
codecvt_utf8<wchar_t>:
UTFW  ==> UTF8 : 0.142860112s (-14.20%)
UTF8  ==> UTFW : 0.703162093s (+675.87%)

*** No errors detected
```
**Attention:** the strange results for UTF16 to UTF8 and UTFW to UTF8 convertions. Strong GCC optimization or bug? Should be investigated in future.

#### Ubuntu 16.04 LTS x64 (Clang v3.9.1):
```cpp
Running 489 test cases...
sizeof wchar_t: 4
UTFW: UTF32
Resolution: 3400053738
UTF8  ==> UTF8 : 0.107137739s
UTF8  ==> UTF16: 0.166798686s
UTF8  ==> UTF32: 0.115869696s
UTF8  ==> UTFW : 0.114985878s
UTF16 ==> UTF8 : 0.179087502s
UTF16 ==> UTF16: 0.060946522s
UTF16 ==> UTF32: 0.071962061s
UTF16 ==> UTFW : 0.071475919s
UTF32 ==> UTF8 : 0.194061658s
UTF32 ==> UTF16: 0.082039203s
UTF32 ==> UTF32: 0.031557019s
UTF32 ==> UTFW : 0.032523089s
UTFW  ==> UTF8 : 0.141759171s
UTFW  ==> UTF16: 0.078305338s
UTFW  ==> UTF32: 0.034137096s
UTFW  ==> UTFW : 0.031711982s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.205740508s (+14.88%)
UTF8  ==> UTF16: 0.272519609s (+63.38%)
codecvt_utf8<wchar_t>:
UTFW  ==> UTF8 : 0.158999648s (+12.16%)
UTF8  ==> UTFW : 0.340384930s (+196.02%)

*** No errors detected
```

#### MacOS High Sierra v10.13.6 (Clang v6.0.0)
```cpp
Running 489 test cases...
sizeof wchar_t: 4
UTFW: UTF32
Resolution: 2793647583
UTF8  ==> UTF8 : 0.111039205s
UTF8  ==> UTF16: 0.143631552s
UTF8  ==> UTF32: 0.105463425s
UTF8  ==> UTFW : 0.105106640s
UTF16 ==> UTF8 : 0.158074631s
UTF16 ==> UTF16: 0.055528284s
UTF16 ==> UTF32: 0.063203264s
UTF16 ==> UTFW : 0.063167823s
UTF32 ==> UTF8 : 0.123977591s
UTF32 ==> UTF16: 0.061630976s
UTF32 ==> UTF32: 0.027633560s
UTF32 ==> UTFW : 0.029324893s
UTFW  ==> UTF8 : 0.123948012s
UTFW  ==> UTF16: 0.064873256s
UTFW  ==> UTF32: 0.030606730s
UTFW  ==> UTFW : 0.027596372s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.151798551s (-3.97%)
UTF8  ==> UTF16: 0.256203078s (+78.38%)
codecvt_utf8<wchar_t>:
UTFW  ==> UTF8 : 0.137034385s (+10.56%)
UTF8  ==> UTFW : 0.360953804s (+243.42%)

*** No errors detected
```

## Conversion table
![UTF-8/32 table](https://upload.wikimedia.org/wikipedia/commons/3/38/UTF-8_Encoding_Scheme.png)

