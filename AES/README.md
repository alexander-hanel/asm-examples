# AES 

The goal of this repository is to aid in understanding how the Advanced Encryption Standard looks when compiled in C++. This repository contains compiled executable files, their corresponding PDB files and a link to the original source code.  It is recommend to download all files, open up the executable in IDA, load the PDB file and then compare the source code. For individuals not familiar with AES, I would recommend checking out the references.  

## References 

[A Stick Figure Guide to the Advanced Encryption Standard (AES)](http://www.moserware.com/2009/09/stick-figure-guide-to-advanced.html)

 - Simple introduction 

[Advanced Encryption Standard](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) (Wikipedia) 
 
 - Expands on the stick figure explanation 
 
[NIST AES](http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf)  

[AES Encryption Videos](https://www.youtube.com/playlist?list=PLKK11LigqitiRH57AbtyJyzsfbNfA8nb-) by [What's a Creel?](https://www.youtube.com/user/WhatsACreel/featured)
 
 - Creating AES from scratch in C++ 
 - Note: [Video 5](https://www.youtube.com/watch?v=4pmR49izUL0) is missing from the playlist.

[Purdue AES Lecture Notes](https://engineering.purdue.edu/kak/compsec/NewLectures/Lecture8.pdf)
 
 - Example code in Python 


## Notes

`In progress, nothing original`

High-level description - [Via Wikipedia](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#High-level_description_of_the_algorithm) 

1. Key Expansion 
2. Initial Round (AddRoundKey)
3. Rounds (SubBytes, ShitRows, MixColumns & AddRoundKeys)
4. Final Round (SubBytes, ShiftRows and AddRoundKeys)


A typical AES function would have a function flow as seen below. This can vary but knowing the function flow can be useful for quickly identifying the functions without understanding the core logic. 
```
AES()
{
	Key_Expansion(key)
	while(n)
		Add_Round_Key()
		Sub_Bytes()
		Shift_Rows()
		Mix_Columns() 

		Add_Rounds()
		Sub_Bytes()
		Shift_Rows()

	return 
}
```

#### Key Expansion 
The key are used by the key expansion to generate a set of Round keys. Round keys are derived from the cipher key using [Rijndael's key schedule](https://en.wikipedia.org/wiki/Rijndael_key_schedule). AES requires a separate 128-bit round key block for each round plus one more. The [keys schedule](https://en.wikipedia.org/wiki/Key_schedule) is used to expand a short key into a number of seperate round keys. It produces the needed round keys from the initial key. The key schedule utilizes a number of operations such as Rotate, Rcon and S-Box. Identifying the expanded key size can be used to identify the size of the key. For example, 128 bit-key uses a key expansion array of 176 bytes, 192 bit-key uses a key expansion array of 208 bytes and 256 bit-key uses a key expansion array of 240 bytes [source](https://www.samiam.org/key-schedule.html). From a reversing perspective, identifying the key expansion function can be used to identify the original key because it is first N bytes of the expanded key. To identify this function search for code cross-references to the S-box or Rcon bytes. 

##### Rotate
The rotate operations takes a `32 bit word (dword)` and rotates it eight bits to the left such that the high eight bits "wrap around" and become the low eight bits of the result. 

##### Rcon 
Rcon is what the Rijndael documentation calls the exponentiation of 2 to a user-specified value. Note that this operation is not performed with regular integers, but in Rijndael's finite field. Only the first some of these constants are actually used – up to rcon[10] for AES-128 (as 11 round keys are needed). Rcon[0] is not used in AES algorithm. The Rijndael variants with larger block sizes use more of these constants, up to rcon[29] for Rijndael with 128-bit keys and 256 bit blocks (needs 15 round keys of each 256 bit, which means 30 full rounds of key expansion, which means 29 calls to the key schedule core using the round constants). The Rcon contains cross references to the key expansion function. 

##### S-box (also known as Rijndael S-box)
The [Rijndael S-box](https://en.wikipedia.org/wiki/Rijndael_S-box) is a `256` byte matrix (square array of numbers). The S-box (substitution box) serves as a lookup table. The S-box is generated by determining the multiplicative inverse for a given number in Rijndael's finite field (also known as Galois field). The multiplcative inverse of two numbers, means the reciprocal (1/number) multiply together to make 1. For example, 1 and -1 are their own reciprocals because 1 × 1 = 1 and -1 × -1 = 1. The S-box contains cross references to the key expansion function and the Sub-Byte

Useful read: [The Laws of Cryptography Advanced Encryption Standard: S-Boxes](http://www.cs.utsa.edu/~wagner/laws/SBoxes.html)


##### AddRounds
Simple XOR loop. The XOR loop is sometimes inline in the main AES function or could be its own function. 

Example Assembly

```
.text:00401000 ; void __cdecl AddRoundKey(char (*state)[4], unsigned int *w)
.text:00401000 _AddRoundKey    proc near               ; CODE XREF: _aes_encrypt+A7↓p
.text:00401000                                         ; _aes_encrypt+DE↓p ...
.text:00401000
.text:00401000 subkey          = byte ptr -4
.text:00401000 var_3           = byte ptr -3
.text:00401000 var_2           = byte ptr -2
.text:00401000 var_1           = byte ptr -1
.text:00401000 state           = dword ptr  8
.text:00401000 w               = dword ptr  0Ch
.text:00401000
.text:00401000                 push    ebp
.text:00401001                 mov     ebp, esp
.text:00401003                 push    ecx
.text:00401004                 mov     eax, [ebp+w]
.text:00401007                 mov     ecx, [eax]
.text:00401009                 shr     ecx, 18h
.text:0040100C                 mov     [ebp+subkey], cl
.text:0040100F                 mov     edx, [ebp+w]
.text:00401012                 mov     eax, [edx]
.text:00401014                 shr     eax, 10h
.text:00401017                 mov     [ebp+var_3], al
.text:0040101A                 mov     ecx, [ebp+w]
.text:0040101D                 mov     edx, [ecx]
.text:0040101F                 shr     edx, 8
.text:00401022                 mov     [ebp+var_2], dl
.text:00401025                 mov     eax, [ebp+w]
.text:00401028                 mov     cl, [eax]
.text:0040102A                 mov     [ebp+var_1], cl
.text:0040102D                 movzx   edx, [ebp+subkey]
.text:00401031                 mov     eax, [ebp+state]
.text:00401034                 movzx   ecx, byte ptr [eax]
.text:00401037                 xor     ecx, edx
.text:00401039                 mov     edx, [ebp+state]
.text:0040103C                 mov     [edx], cl
.text:0040103E                 movzx   eax, [ebp+var_3]
.text:00401042                 mov     ecx, [ebp+state]
.text:00401045                 movzx   edx, byte ptr [ecx+4]
.text:00401049                 xor     edx, eax
.text:0040104B                 mov     eax, [ebp+state]
.text:0040104E                 mov     [eax+4], dl
.text:00401051                 movzx   ecx, [ebp+var_2]
.text:00401055                 mov     edx, [ebp+state]
.text:00401058                 movzx   eax, byte ptr [edx+8]
.text:0040105C                 xor     eax, ecx
.text:0040105E                 mov     ecx, [ebp+state]
.text:00401061                 mov     [ecx+8], al
.text:00401064                 movzx   edx, [ebp+var_1]
.text:00401068                 mov     eax, [ebp+state]
.text:0040106B                 movzx   ecx, byte ptr [eax+0Ch]
.text:0040106F                 xor     ecx, edx
.text:00401071                 mov     edx, [ebp+state]
.text:00401074                 mov     [edx+0Ch], cl
.text:00401077                 mov     eax, [ebp+w]
.text:0040107A                 mov     ecx, [eax+4]
.text:0040107D                 shr     ecx, 18h
.text:00401080                 mov     [ebp+subkey], cl
.text:00401083                 mov     edx, [ebp+w]
.text:00401086                 mov     eax, [edx+4]
.text:00401089                 shr     eax, 10h
.text:0040108C                 mov     [ebp+var_3], al
.text:0040108F                 mov     ecx, [ebp+w]
.text:00401092                 mov     edx, [ecx+4]
.text:00401095                 shr     edx, 8
.text:00401098                 mov     [ebp+var_2], dl
.text:0040109B                 mov     eax, [ebp+w]
.text:0040109E                 mov     cl, [eax+4]
.text:004010A1                 mov     [ebp+var_1], cl
.text:004010A4                 movzx   edx, [ebp+subkey]
.text:004010A8                 mov     eax, [ebp+state]
.text:004010AB                 movzx   ecx, byte ptr [eax+1]
.text:004010AF                 xor     ecx, edx
.text:004010B1                 mov     edx, [ebp+state]
.text:004010B4                 mov     [edx+1], cl
.text:004010B7                 movzx   eax, [ebp+var_3]
.text:004010BB                 mov     ecx, [ebp+state]
.text:004010BE                 movzx   edx, byte ptr [ecx+5]
.text:004010C2                 xor     edx, eax
.text:004010C4                 mov     eax, [ebp+state]
.text:004010C7                 mov     [eax+5], dl
.text:004010CA                 movzx   ecx, [ebp+var_2]
.text:004010CE                 mov     edx, [ebp+state]
.text:004010D1                 movzx   eax, byte ptr [edx+9]
.text:004010D5                 xor     eax, ecx
.text:004010D7                 mov     ecx, [ebp+state]
.text:004010DA                 mov     [ecx+9], al
.text:004010DD                 movzx   edx, [ebp+var_1]
.text:004010E1                 mov     eax, [ebp+state]
.text:004010E4                 movzx   ecx, byte ptr [eax+0Dh]
.text:004010E8                 xor     ecx, edx
.text:004010EA                 mov     edx, [ebp+state]
.text:004010ED                 mov     [edx+0Dh], cl
.text:004010F0                 mov     eax, [ebp+w]
.text:004010F3                 mov     ecx, [eax+8]
.text:004010F6                 shr     ecx, 18h
.text:004010F9                 mov     [ebp+subkey], cl
.text:004010FC                 mov     edx, [ebp+w]
.text:004010FF                 mov     eax, [edx+8]
.text:00401102                 shr     eax, 10h
.text:00401105                 mov     [ebp+var_3], al
.text:00401108                 mov     ecx, [ebp+w]
.text:0040110B                 mov     edx, [ecx+8]
.text:0040110E                 shr     edx, 8
.text:00401111                 mov     [ebp+var_2], dl
.text:00401114                 mov     eax, [ebp+w]
.text:00401117                 mov     cl, [eax+8]
.text:0040111A                 mov     [ebp+var_1], cl
.text:0040111D                 movzx   edx, [ebp+subkey]
.text:00401121                 mov     eax, [ebp+state]
.text:00401124                 movzx   ecx, byte ptr [eax+2]
.text:00401128                 xor     ecx, edx
.text:0040112A                 mov     edx, [ebp+state]
.text:0040112D                 mov     [edx+2], cl
.text:00401130                 movzx   eax, [ebp+var_3]
.text:00401134                 mov     ecx, [ebp+state]
.text:00401137                 movzx   edx, byte ptr [ecx+6]
.text:0040113B                 xor     edx, eax
.text:0040113D                 mov     eax, [ebp+state]
.text:00401140                 mov     [eax+6], dl
.text:00401143                 movzx   ecx, [ebp+var_2]
.text:00401147                 mov     edx, [ebp+state]
.text:0040114A                 movzx   eax, byte ptr [edx+0Ah]
.text:0040114E                 xor     eax, ecx
.text:00401150                 mov     ecx, [ebp+state]
.text:00401153                 mov     [ecx+0Ah], al
.text:00401156                 movzx   edx, [ebp+var_1]
.text:0040115A                 mov     eax, [ebp+state]
.text:0040115D                 movzx   ecx, byte ptr [eax+0Eh]
.text:00401161                 xor     ecx, edx
.text:00401163                 mov     edx, [ebp+state]
.text:00401166                 mov     [edx+0Eh], cl
.text:00401169                 mov     eax, [ebp+w]
.text:0040116C                 mov     ecx, [eax+0Ch]
.text:0040116F                 shr     ecx, 18h
.text:00401172                 mov     [ebp+subkey], cl
.text:00401175                 mov     edx, [ebp+w]
.text:00401178                 mov     eax, [edx+0Ch]
.text:0040117B                 shr     eax, 10h
.text:0040117E                 mov     [ebp+var_3], al
.text:00401181                 mov     ecx, [ebp+w]
.text:00401184                 mov     edx, [ecx+0Ch]
.text:00401187                 shr     edx, 8
.text:0040118A                 mov     [ebp+var_2], dl
.text:0040118D                 mov     eax, [ebp+w]
.text:00401190                 mov     cl, [eax+0Ch]
.text:00401193                 mov     [ebp+var_1], cl
.text:00401196                 movzx   edx, [ebp+subkey]
.text:0040119A                 mov     eax, [ebp+state]
.text:0040119D                 movzx   ecx, byte ptr [eax+3]
.text:004011A1                 xor     ecx, edx
.text:004011A3                 mov     edx, [ebp+state]
.text:004011A6                 mov     [edx+3], cl
.text:004011A9                 movzx   eax, [ebp+var_3]
.text:004011AD                 mov     ecx, [ebp+state]
.text:004011B0                 movzx   edx, byte ptr [ecx+7]
.text:004011B4                 xor     edx, eax
.text:004011B6                 mov     eax, [ebp+state]
.text:004011B9                 mov     [eax+7], dl
.text:004011BC                 movzx   ecx, [ebp+var_2]
.text:004011C0                 mov     edx, [ebp+state]
.text:004011C3                 movzx   eax, byte ptr [edx+0Bh]
.text:004011C7                 xor     eax, ecx
.text:004011C9                 mov     ecx, [ebp+state]
.text:004011CC                 mov     [ecx+0Bh], al
.text:004011CF                 movzx   edx, [ebp+var_1]
.text:004011D3                 mov     eax, [ebp+state]
.text:004011D6                 movzx   ecx, byte ptr [eax+0Fh]
.text:004011DA                 xor     ecx, edx
.text:004011DC                 mov     edx, [ebp+state]
.text:004011DF                 mov     [edx+0Fh], cl
.text:004011E2                 mov     esp, ebp
.text:004011E4                 pop     ebp
.text:004011E5                 retn
.text:004011E5 _AddRoundKey    endp
```
Example 2. 

```
.text:004026C0 ; void __thiscall Aes256::add_round_key(Aes256 *this, char *buffer, const char round)
.text:004026C0 ?add_round_key@Aes256@@AAEXPAEE@Z proc near
.text:004026C0                                         ; CODE XREF: Aes256::encrypt(uchar *)+1A↑p
.text:004026C0                                         ; Aes256::encrypt(uchar *)+80↑p ...
.text:004026C0
.text:004026C0 _Pos            = dword ptr -0Ch
.text:004026C0 this            = dword ptr -8
.text:004026C0 i               = byte ptr -1
.text:004026C0 buffer          = dword ptr  8
.text:004026C0 round           = byte ptr  0Ch
.text:004026C0
.text:004026C0                 push    ebp
.text:004026C1                 mov     ebp, esp
.text:004026C3                 sub     esp, 0Ch
.text:004026C6                 push    esi
.text:004026C7                 mov     [ebp+this], ecx
.text:004026CA                 mov     [ebp+i], 10h
.text:004026CE
.text:004026CE loc_4026CE:                             ; CODE XREF: Aes256::add_round_key(uchar *,uchar)+64↓j
.text:004026CE                 movzx   eax, [ebp+i]
.text:004026D2                 mov     cl, [ebp+i]
.text:004026D5                 sub     cl, 1
.text:004026D8                 mov     [ebp+i], cl
.text:004026DB                 test    eax, eax
.text:004026DD                 jz      short loc_402726
.text:004026DF                 movzx   edx, [ebp+round]
.text:004026E3                 and     edx, 1
.text:004026E6                 jz      short loc_4026F4
.text:004026E8                 movzx   eax, [ebp+i]
.text:004026EC                 add     eax, 10h
.text:004026EF                 mov     [ebp+_Pos], eax
.text:004026F2                 jmp     short loc_4026FB
.text:004026F4 ; ---------------------------------------------------------------------------
.text:004026F4
.text:004026F4 loc_4026F4:                             ; CODE XREF: Aes256::add_round_key(uchar *,uchar)+26↑j
.text:004026F4                 movzx   ecx, [ebp+i]
.text:004026F8                 mov     [ebp+_Pos], ecx
.text:004026FB
.text:004026FB loc_4026FB:                             ; CODE XREF: Aes256::add_round_key(uchar *,uchar)+32↑j
.text:004026FB                 movzx   esi, [ebp+i]
.text:004026FF                 mov     edx, [ebp+_Pos]
.text:00402702                 push    edx             ; _Pos
.text:00402703                 mov     ecx, [ebp+this]
.text:00402706                 add     ecx, 30h ; '0'  ; this
.text:00402709                 call    ??A?$vector@EV?$allocator@E@std@@@std@@QAEAAEI@Z ; std::vector<uchar>::operator[](uint)
.text:0040270E                 movzx   eax, byte ptr [eax]
.text:00402711                 mov     ecx, [ebp+buffer]
.text:00402714                 movzx   edx, byte ptr [ecx+esi]
.text:00402718                 xor     edx, eax
.text:0040271A                 movzx   eax, [ebp+i]
.text:0040271E                 mov     ecx, [ebp+buffer]
.text:00402721                 mov     [ecx+eax], dl
.text:00402724                 jmp     short loc_4026CE
.text:00402726 ; ---------------------------------------------------------------------------
.text:00402726
.text:00402726 loc_402726:                             ; CODE XREF: Aes256::add_round_key(uchar *,uchar)+1D↑j
.text:00402726                 pop     esi
.text:00402727                 mov     esp, ebp
.text:00402729                 pop     ebp
.text:0040272A                 retn    8
.text:0040272A ?add_round_key@Aes256@@AAEXPAEE@Z endp
```

##### SubBytes



##### ShitRows



##### MixColumns