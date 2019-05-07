#### MacOS Mojave v10.14.4 x64 (Apple Clang v10.0.1):
```
Running 498 test cases...
compiler: Apple Clang v10.0.1
architecture: x64
__cpp_lib_string_view: 201103
sizeof wchar_t: 4
UTFW: UTF32
Resolution: 2793641013
UTF8  ==> UTF8 : 0.089639617s
UTF8  ==> UTF16: 0.176716818s
UTF8  ==> UTF32: 0.116081138s
UTF8  ==> UTFW : 0.116293182s
UTF16 ==> UTF8 : 0.157785545s
UTF16 ==> UTF16: 0.050479281s
UTF16 ==> UTF32: 0.088360189s
UTF16 ==> UTFW : 0.089149971s
UTF32 ==> UTF8 : 0.117630715s
UTF32 ==> UTF16: 0.110660859s
UTF32 ==> UTF32: 0.029954593s
UTF32 ==> UTFW : 0.045362425s
UTFW  ==> UTF8 : 0.117514776s
UTFW  ==> UTF16: 0.110773972s
UTFW  ==> UTF32: 0.045098655s
UTFW  ==> UTFW : 0.029025044s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.097068829s (-38.48%)
UTF8  ==> UTF16: 0.211710073s (+19.80%)
codecvt_utf8<wchar_t>:
UTFW  ==> UTF8 : 0.108117083s (-8.00%)
UTF8  ==> UTFW : 0.295663358s (+154.24%)

*** No errors detected
```
**Attention:** the strange results for UTF16 to UTF8 and UTFW to UTF8 convertions. I suppose that there is no input data checking in codecvt library.
