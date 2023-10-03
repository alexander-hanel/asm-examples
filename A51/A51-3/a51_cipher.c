#include <stdio.h>
#include <stdlib.h>
// #include <stdbool.h>

typedef int bool;
#define false 0
#define true 1

#define R1_CLOCK_BIT 8
#define R2_CLOCK_BIT 10
#define R3_CLOCK_BIT 10

#define R1_MASK 0x7FFFF  /* 19 bits all 1s */
#define R2_MASK 0x3FFFFF /* 22 bits all 1s */
#define R3_MASK 0x7FFFFF /* 23 bits all 1s */

#define R1_TAP_1 13
#define R1_TAP_2 16
#define R1_TAP_3 17
#define R1_TAP_4 18

#define R2_TAP_1 20
#define R2_TAP_2 21

#define R3_TAP_1 7
#define R3_TAP_2 20
#define R3_TAP_3 21
#define R3_TAP_4 22

#define N_BITS 114

#define GET_BIT(num, i) ((num >> i) & 1)

#define XOR_MSB(R1, R2, R3) (((GET_BIT(R1, 18) ^ GET_BIT(R2, 21) ^ GET_BIT(R3, 22))) & 1)

/*
This function calculates the majority between the three registers
*/
bool majority(unsigned long R1, unsigned long R2, unsigned long R3)
{
  unsigned char r1_clock = GET_BIT(R1, R1_CLOCK_BIT);
  unsigned char r2_clock = GET_BIT(R2, R2_CLOCK_BIT);
  unsigned char r3_clock = GET_BIT(R3, R3_CLOCK_BIT);
  if (r1_clock + r2_clock + r3_clock >= 2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void clock(unsigned long *R, int num_reg)
{
  bool a, b, c, d;
  if (num_reg == 1)
  {
    a = GET_BIT(*R, R1_TAP_1);
    b = GET_BIT(*R, R1_TAP_2);
    c = GET_BIT(*R, R1_TAP_3);
    d = GET_BIT(*R, R1_TAP_4);
    a ^= b;
    a ^= c;
    a ^= d;
    // shift left and keep the right amount of bits
    *R = (*R << 1) & R1_MASK;
    // insert new bit, the shift brings a 0 in. Modify it if needed.
    if(a==1)
      *R |= 1;
  }
  else if (num_reg == 2)
  {
    a = GET_BIT(*R, R2_TAP_1);
    b = GET_BIT(*R, R2_TAP_2);
    a ^= b;
    // shift left and keep the right amount of bits
    *R = (*R << 1) & R2_MASK;
    // insert new bit, the shift brings a 0 in. Modify it if needed.
    if(a==1)
      *R |= 1;
  }
  else
  {
    a = GET_BIT(*R, R3_TAP_4);
    b = GET_BIT(*R, R3_TAP_3);
    c = GET_BIT(*R, R3_TAP_2);
    d = GET_BIT(*R, R3_TAP_1);
    a ^= b;
    a ^= c;
    a ^= d;
    // shift left and keep the right amount of bits
    *R = (*R << 1) & R3_MASK;
    // insert new bit, the shift brings a 0 in. Modify it if needed.
    if(a==1)
      *R |= 1;
  }
}

/*
This function implements the clocking operation for the register passed as argument
The maj_rule variable is used to differentiate between the initial clocking operation,
which doesn't uses the clocking bits of the registers, between the regular clocking operations.
*/
void clock_register(unsigned long *ptr_R1, unsigned long *ptr_R2, unsigned long *ptr_R3, bool maj_rule)
{
  if (maj_rule == 0)
  {
    clock(ptr_R1, 1);
    clock(ptr_R2, 2);
    clock(ptr_R3, 3);
  }
  else
  {
    bool maj = majority(*ptr_R1, *ptr_R2, *ptr_R3);
    if ((GET_BIT(*ptr_R1, R1_CLOCK_BIT)) == maj)
    {
      clock(ptr_R1, 1);
    }
    if ((GET_BIT(*ptr_R2, R2_CLOCK_BIT)) == maj)
    {
      clock(ptr_R2, 2);
    }
    if ((GET_BIT(*ptr_R3, R3_CLOCK_BIT)) == maj)
    {
      clock(ptr_R3, 3);
    }
  }
}

/*
This function does the initial setup of the registers using the 64-bits key and the 22-bits frame number.
*/
void key_init(unsigned long *ptr_R1, unsigned long *ptr_R2, unsigned long *ptr_R3, unsigned long frame, unsigned char key[8])
{
  int i;
  bool key_bit;
  bool frame_bit;
  for (i = 0; i < 64; i++)
  {
    clock_register(ptr_R1, ptr_R2, ptr_R3, 0);
    key_bit = GET_BIT(key[i / 8], (i & 7));
    *ptr_R1 ^= key_bit;
    *ptr_R2 ^= key_bit;
    *ptr_R3 ^= key_bit;
  }

  for (i = 0; i < 22; i++)
  {
    clock_register(ptr_R1, ptr_R2, ptr_R3, 0);
    frame_bit = GET_BIT(frame, i);
    *ptr_R1 ^= frame_bit;
    *ptr_R2 ^= frame_bit;
    *ptr_R3 ^= frame_bit;
  }

  for (i = 0; i < 100; i++)
  {
    clock_register(ptr_R1, ptr_R2, ptr_R3, 1);
  }

  printf("[-] Key setup completed.\n");
  printf("[-] R1=%ld, R2=%ld, R3=%ld \n", *ptr_R1, *ptr_R2, *ptr_R3);
}

void encrypt_frame(unsigned long *ptr_R1, unsigned long *ptr_R2, unsigned long *ptr_R3, unsigned char *downlink, unsigned char *uplink)
{
  int i;
  for (i = 0; i < N_BITS / 8; i++)
  {
    downlink[i] = 0;
    uplink[i] = 0;
  }

  // downlink
  for (i = 0; i < N_BITS; i++)
  {
    clock_register(ptr_R1, ptr_R2, ptr_R3, 1);
    downlink[i / 8] |= (XOR_MSB(*ptr_R1, *ptr_R2, *ptr_R3)) << (7 - (i & 7));
  }

  // uplink
  for (i = 0; i < N_BITS; i++)
  {
    clock_register(ptr_R1, ptr_R2, ptr_R3, 1);
    uplink[i / 8] |= (XOR_MSB(*ptr_R1, *ptr_R2, *ptr_R3)) << (7 - (i & 7));
  }

  printf("[-] Frame successfully encrypted\n");
  return;
}

void main()
{
  unsigned long R1 = 0L;
  unsigned long R2 = 0L;
  unsigned long R3 = 0L;
  unsigned char key[8] = {0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
  unsigned long frame = 0x134L;
  unsigned long *ptr_R1 = &R1;
  unsigned long *ptr_R2 = &R2;
  unsigned long *ptr_R3 = &R3;
  unsigned char uplink[15];
  unsigned char downlink[15];
  int i;

  printf("key: 0x");
  for (i = 0; i < 8; i++)
    printf("%02X", key[i]);
  printf("\n");
  printf("frame number: 0x%06X\n", (unsigned int)frame);
  key_init(ptr_R1, ptr_R2, ptr_R3, frame, key);
  printf("[-] Encrypting frame...\n");
  encrypt_frame(ptr_R1, ptr_R2, ptr_R3, downlink, uplink);
}