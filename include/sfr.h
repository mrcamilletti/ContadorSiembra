#ifndef FD81F02E_657C_4CD1_B385_729BD1B1C2E6
#define FD81F02E_657C_4CD1_B385_729BD1B1C2E6

/* From: https://forum.arduino.cc/t/trick-for-getting-port-sfr_io-address/392012 */

#include <stdint.h>

#define sfr_addr(sfr) reinterpret_cast<unsigned> ((volatile void *)(&sfr))

template < const uint8_t sfr, const uint8_t bit>
struct sfr_bit {
	inline void high() { *(uint8_t volatile *)sfr |= 1<<bit;}
	inline void low()  { *(uint8_t volatile *)sfr &= ~(1<<bit);}
	inline unsigned read() { 
		if (*(uint8_t volatile *)sfr & 1<<bit) return 1; else return 0; }
	inline  operator unsigned () { return read(); }
	inline sfr_bit operator =(const unsigned value) {
		if (value==0) low(); else high(); return *this;  }
};

#endif /* FD81F02E_657C_4CD1_B385_729BD1B1C2E6 */
