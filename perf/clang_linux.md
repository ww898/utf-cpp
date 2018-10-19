#### Ubuntu 16.04 LTS x64 (Clang v6.0.1):
```
Running 498 test cases...
compiler: Clang v6.0.1
architecture: x64
__cpp_lib_string_view: 201103
sizeof wchar_t: 4
UTFW: UTF32
Resolution: 3400053429
UTF8  ==> UTF8 : 0.107418207s
UTF8  ==> UTF16: 0.188430360s
UTF8  ==> UTF32: 0.134659423s
UTF8  ==> UTFW : 0.135117026s
UTF16 ==> UTF8 : 0.228770144s
UTF16 ==> UTF16: 0.058630704s
UTF16 ==> UTF32: 0.092677242s
UTF16 ==> UTFW : 0.090311157s
UTF32 ==> UTF8 : 0.161631558s
UTF32 ==> UTF16: 0.087601148s
UTF32 ==> UTF32: 0.035452413s
UTF32 ==> UTFW : 0.036947061s
UTFW  ==> UTF8 : 0.161716769s
UTFW  ==> UTF16: 0.086968201s
UTFW  ==> UTF32: 0.036887902s
UTFW  ==> UTFW : 0.035122482s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.141921857s (-37.96%)
UTF8  ==> UTF16: 0.252857462s (+34.19%)
codecvt_utf8<wchar_t>:
UTFW  ==> UTF8 : 0.159812705s (-1.18%)
UTF8  ==> UTFW : 0.340444965s (+151.96%)

*** No errors detected
```
*Attention:* the strange results for UTF16 to UTF8 and UTFW to UTF8 convertions. I suppose that there is no input data checking in codecvt library.
