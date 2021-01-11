# Salsa 

Compiled executables with corresponding PDBs. 

# References 

[Salsa20](https://en.wikipedia.org/wiki/Salsa20) (Wikipedia)
- Salsa20 and the closely related ChaCha are stream ciphers developed by Daniel J. Bernstein. 
- Both ciphers are built on a pseudorandom function based on add-rotate-XOR (ARX) operations — 32-bit addition, bitwise addition (XOR) and rotation operations. The core function maps a 256-bit key, a 64-bit nonce (number used once), and a 64-bit counter to a 512-bit block of the key stream (a Salsa version with a 128-bit key also exists).
- Salsa20 performs 20 rounds of mixing on its input.[1] However, reduced round variants Salsa20/8 and Salsa20/12 using 8 and 12 rounds respectively have also been introduced. 

[An Overview of eSTREAM Ciphers](https://pratmukh.files.wordpress.com/2017/03/estream.pdf) pg. 43
- The core of Salsa20 is a hash function with 64-byte input and 64-byte output. The hash function
is used in counter mode as a stream cipher: Salsa20 encrypts a 64-byte block of plaintext by hashing the key, nonce, and block number and xor’ing the result with the plaintext.

[Snuffle](http://cr.yp.to/snuffle/design.pdf)
- Awesome read. 


[On the Salsa20 Core Function](https://www.iacr.org/archive/fse2008/50860470/50860470.pdf)
 - Salsa20 represents quite an original and flexible design, where the author
justifies the use of very simple operations (addition, xor, constant distance rotation) and the lack of multiplication or S-boxes to develop a very fast primitive.
 
# Notes

High-level description/components  

### quarterround

The core operation in Salsa20 is the quarter-round QR(a, b, c, d) that takes a four-word input and produces a four-word output:
```
b ^= (a + d) <<< 7;
c ^= (b + a) <<< 9;
d ^= (c + b) <<< 13;
a ^= (d + c) <<< 18;
```
The values `7, 9. 13, 18` can be used to identify the quarterround functionality. For example they are present in the below assembly as the Shift value. 
```
.text:00411E0E                 push    7               ; Shift
.text:00411E10                 mov     eax, [ebp+x]
.text:00411E13                 add     eax, [ebp+x+30h]
.text:00411E16                 push    eax             ; Value
.text:00411E17                 call    j___lrotl
.text:00411E1C                 add     esp, 8
.text:00411E1F                 xor     eax, [ebp+x+10h]
.text:00411E22                 mov     [ebp+x+10h], eax
.text:00411E25                 push    9               ; Shift
.text:00411E27                 mov     eax, [ebp+x+10h]
.text:00411E2A                 add     eax, [ebp+x]
.text:00411E2D                 push    eax             ; Value
.text:00411E2E                 call    j___lrotl
.text:00411E33                 add     esp, 8
.text:00411E36                 xor     eax, [ebp+x+20h]
.text:00411E39                 mov     [ebp+x+20h], eax
.text:00411E3C                 push    13              ; Shift
.text:00411E3E                 mov     eax, [ebp+x+20h]
.text:00411E41                 add     eax, [ebp+x+10h]
.text:00411E44                 push    eax             ; Value
.text:00411E45                 call    j___lrotl
.text:00411E4A                 add     esp, 8
.text:00411E4D                 xor     eax, [ebp+x+30h]
.text:00411E50                 mov     [ebp+x+30h], eax
.text:00411E53                 push    18              ; Shift
.text:00411E55                 mov     eax, [ebp+x+30h]
```

### columnround
Odd-numbered rounds apply QR(a, b, c, d) to each of the four columns in the 4×4 matrix, and even-numbered rounds apply it to each of the four rows. Two consecutive rounds (column-round and row-round) together are called a double-round:

```
// Odd round
QR( 0,  4,  8, 12)	// column 1
QR( 5,  9, 13,  1)	// column 2
QR(10, 14,  2,  6)	// column 3
QR(15,  3,  7, 11)	// column 4
// Even round
QR( 0,  1,  2,  3)	// row 1
QR( 5,  6,  7,  4)	// row 2
QR(10, 11,  8,  9)	// row 3
QR(15, 12, 13, 14)	// row 4
```

### doubleround
The doubleround function is simply a columnround followed by a rowround. 




