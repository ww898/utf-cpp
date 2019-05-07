#### Ubuntu 16.04 LTS x64 (GNU v5.4.0):
```
Running 498 test cases...
compiler: GNU v5.4.0
architecture: x64
__cpp_lib_string_view: 201103
sizeof wchar_t: 4
UTFW: UTF32
Resolution: 3400051831
UTF8  ==> UTF8 : 0.102757740s
UTF8  ==> UTF16: 0.095532425s
UTF8  ==> UTF32: 0.068401628s
UTF8  ==> UTFW : 0.068429746s
UTF16 ==> UTF8 : 0.145150758s
UTF16 ==> UTF16: 0.059405159s
UTF16 ==> UTF32: 0.041120038s
UTF16 ==> UTFW : 0.041308014s
UTF32 ==> UTF8 : 0.152833210s
UTF32 ==> UTF16: 0.061162408s
UTF32 ==> UTF32: 0.034944571s
UTF32 ==> UTFW : 0.032738255s
UTFW  ==> UTF8 : 0.156477637s
UTFW  ==> UTF16: 0.060663657s
UTFW  ==> UTF32: 0.032810332s
UTFW  ==> UTFW : 0.035520935s
codecvt_utf8_utf16<char16_t>:
UTF16 ==> UTF8 : 0.168646978s (+16.19%)
UTF8  ==> UTF16: 0.411385577s (+330.62%)
codecvt_utf8<wchar_t>:
UTFW  ==> UTF8 : 0.148243451s (-5.26%)
UTF8  ==> UTFW : 0.674670587s (+885.93%)

*** No errors detected
```
**Attention:** the strange results for UTFW to UTF8 convertion. I suppose that there is no input data checking in codecvt library.
