#include "pifacedigital.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int pifacedigital_open(uint8_t hw_addr){ return 0; }
void pifacedigital_close(uint8_t hw_addr) { return ; }
uint8_t pifacedigital_read_reg(uint8_t reg, uint8_t hw_addr){
   uint8_t btns;
   FILE *fp;
   fp = fopen("buttons.dat", "r");
   if( fp ) {
      fread( &btns, sizeof(btns), 1, fp);
      fclose(fp);
      return btns;
   }
   return 0xff;
}
int pifacedigital_enable_interrupts(){return 0;}


