#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


int pifacedigital_open(uint8_t hw_addr);
void pifacedigital_close(uint8_t hw_add);
uint8_t pifacedigital_read_reg(uint8_t reg, uint8_t hw_addr);
int pifacedigital_enable_interrupts();

#ifdef __cplusplus
}
#endif
