# A5/1 

> __Note__: In progress. Commiting so I don't lose it. This project got interrupted by a a second baby a couple of months back. I can't remember where I left off...

The goal of this directory is to aid in understanding how A5/1 looks when compiled in C or C++. This repository contains compiled executable files, their corresponding PDB files and a link to the original source code. It is recommend to download all files, open up the executable in IDA, load the PDB file and then compare the source code. For individuals not familiar with A5/1, I would recommend checking out the [References](#references).

> A5/1 is a stream cipher used to provide over-the-air communication privacy in the GSM cellular telephone standard. It is one of several implementations of the A5 security protocol. It was initially kept secret, but became public knowledge through leaks and reverse engineering. A number of serious weaknesses in the cipher have been identified. 

via [Wikipedia]() 

# Background aka The “TMZ” of A5/1 

At first glance there isn’t anything even that interesting about A5/1 or it's Wikipedia [page](https://en.wikipedia.org/wiki/A5/1) but once you start reading it turns into a fascinating story of Cold War influences, 90’s reverse engineering, GSM phones and the fallout of using security through obscurity.  The following are points on A5/1 history that I found interesting. If you want to dig deeper, I’d recommend reading through the linked Resources. 

To understand how A5/1 fits into the bigger pictures, it is useful to describe a little bit about [GSM](ttps://en.wikipedia.org/wiki/GSM). In 1982 experts from all over Europe came together to build mobile networks. The system was created in 1992 and is now called GSM.  It uses several cryptographic algorithms:
* A5/1 is the "strong" over-the-air voice-privacy algorithm.
* A5/2 is the "weak" over-the-air voice-privacy algorithm.
* A3 is the authentication algorithm.
* A8 is used to generate the session key used by A5 to encrypt the data.
* Both A3 and A8 use [COMP128](https://en.wikipedia.org/wiki/COMP128).

The original encryption key for A5/1 was proposed to be 128 bits. The assumption with using 128 bits is that A5/1 would be secure for at least 15 years. Instead, the encryption key was 54 bits. There is speculation that a 128 bit could have had performance issues. Outside of performance one reported reason of a key size of 54 bits was because of political pressure. To understand why there was pressure, it is helpful to realize that GSM was being developed towards the end of the Cold War, in which espionage was a powerful intelligence tool. Of the reports, there is a discussion of The British Government protested a 128 bit can and wanted to weaken the key to be 48 bits so they could eavesdrop more easily. Another report described that West Germany protested a 48-bit key because they wanted a stronger encryption to prevent spying from East Germany. 

The result was a 54-bit key. Even though GSM supports encryption, the specification states that encryption can be turned off due to a request by some countries.  It's also worth noting that during the 80s and early 90s, if the key was stronger, cell phones could not have been brought into Eastern Europe due to export controls on encryption.

Originally the encryption algorithms were kept secret. In 1994 a leaked image released some specifications of the A5/1, but it wasn't until 1999 that A5/1 became publicly available by individuals who reverse engineered the algorithm from a GSM phone. Disclaimer: I emailed one of the individuals who reverse engineered the algorithms because I was curious about their methodology and tooling used but I never received a reply. In one [email](https://cryptome.org/jya/gsm-weak.htm) sent to a mailing list in October of 1999, the reverse engineering process was described to have taken place “during evenings and on weekends over the course of a few months on a budget of well below $100”. The cryptanalysis took “2 hours (COMP128) and 2 days (A5/2, and that included coding up the attack) to find”. After the source code of the cryptographic algorithms were released several flaws and attacks were found. A5/2 was [withdrawn](https://web.archive.org/web/20110517151812/security.osmocom.org/trac/wiki/A52_Withdrawal) from the GSM specification with manufacturers told to remove or disable it from GSM phones, terminals, and networks.   


# References 

- [A5/1](https://en.wikipedia.org/wiki/A5/1) (Wikipedia)

- [A Pedagogical Implementation of A5/1](https://cryptome.org/jya/a51-pi.htm)

- [Sources: We were pressured to weaken the mobile security in the 80's](https://www.aftenposten.no/verden/i/Olkl/sources-we-were-pressured-to-weaken-the-mobile-security-in-the-80s)
- [A5 (Was: HACKING DIGITAL PHONES)](https://web.archive.org/web/20121106123514/http:/groups.google.com/group/uk.telecom/msg/ba76615fef32ba32)
- [RE: GSM security questions](https://cryptome.org/jya/gsm-weak.htm)
- [ Real Time Cryptanalysis of A5/1 on a PC ](https://cryptome.org/a51-bsw.htm)
- [A brief history on the withdrawal of the A5/2 ciphering algorithm in GSM](https://laforge.gnumonks.org/blog/20101112-history_of_a52_withdrawal/)
# Notes

### Values 
The following are distinctive values that can aid in identifying A5/1.

```c
/* Masks for the three shift registers */
#define R1MASK	0x07FFFF /* 19 bits, numbered 0..18 */
#define R2MASK	0x3FFFFF /* 22 bits, numbered 0..21 */
#define R3MASK	0x7FFFFF /* 23 bits, numbered 0..22 */

/* Middle bit of each of the three shift registers, for clock control */
#define R1MID	0x000100 /* bit 8 */
#define R2MID	0x000400 /* bit 10 */
#define R3MID	0x000400 /* bit 10 */

/* Feedback taps, for clocking the shift registers.
 * These correspond to the primitive polynomials
 * x^19 + x^5 + x^2 + x + 1, x^22 + x + 1,
 * and x^23 + x^15 + x^2 + x + 1. */
#define R1TAPS	0x072000 /* bits 18,17,16,13 */
#define R2TAPS	0x300000 /* bits 21,20 */
#define R3TAPS	0x700080 /* bits 22,21,20,7 */

/* Output taps, for output generation */
#define R1OUT	0x040000 /* bit 18 (the high bit) */
#define R2OUT	0x200000 /* bit 21 (the high bit) */
#define R3OUT	0x400000 /* bit 22 (the high bit) */
```

The masks and taps appear in the clock functions
```assemblly
.text:00401220
.text:00401220 _clockallthree  proc near               ; CODE XREF: j__clockallthree↑j
.text:00401220                 push    ebp
.text:00401221                 mov     ebp, esp
.text:00401223                 push    72000h          ; taps
.text:00401228                 push    7FFFFh          ; mask
.text:0040122D                 mov     eax, _R1
.text:00401232                 push    eax             ; reg
.text:00401233                 call    j__clockone
.text:00401238                 add     esp, 0Ch
.text:0040123B                 mov     _R1, eax
.text:00401240                 push    300000h         ; taps
.text:00401245                 push    3FFFFFh         ; mask
.text:0040124A                 mov     ecx, _R2
.text:00401250                 push    ecx             ; reg
.text:00401251                 call    j__clockone
.text:00401256                 add     esp, 0Ch
.text:00401259                 mov     _R2, eax
.text:0040125E                 push    700080h         ; taps
.text:00401263                 push    7FFFFFh         ; mask
.text:00401268                 mov     edx, _R3
.text:0040126E                 push    edx             ; reg
.text:0040126F                 call    j__clockone
.text:00401274                 add     esp, 0Ch
.text:00401277                 mov     _R3, eax
.text:0040127C                 pop     ebp
.text:0040127D                 retn
.text:0040127D _clockallthree  endp
```
## Parity 
TODO 

## What is an LSFR (linear feedback shift register)?
TODO
