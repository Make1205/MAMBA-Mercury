#include "randombytes_api_pkc.h"
#include "../drng.h"
extern DRNG_ctx drng_algorithm;
void randombytes(unsigned char *buf, unsigned long long len){ get_random_number(&drng_algorithm, buf, len*8); }
