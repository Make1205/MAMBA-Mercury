#include "nike.h"

const nike_params NIKE_128 = {"MAMBA-NIKE-128", 128, 32, NEWHOPE_SENDABYTES, NEWHOPE_SENDBBYTES};
const nike_params NIKE_192 = {"MAMBA-NIKE-192", 192, 32, NEWHOPE_SENDABYTES, NEWHOPE_SENDBBYTES};
const nike_params NIKE_256 = {"MAMBA-NIKE-256", 256, 32, NEWHOPE_SENDABYTES, NEWHOPE_SENDBBYTES};
const nike_params NIKE_384 = {"MAMBA-NIKE-384", 384, 32, NEWHOPE_SENDABYTES, NEWHOPE_SENDBBYTES};
const nike_params NIKE_512 = {"MAMBA-NIKE-512", 512, 32, NEWHOPE_SENDABYTES, NEWHOPE_SENDBBYTES};

size_t nike_ma_bytes(const nike_params *p){ return p->ma_bytes; }
size_t nike_mb_bytes(const nike_params *p){ return p->mb_bytes; }

void nike_init(nike_state *st, unsigned char *M_A, const nike_params *p){
  (void)p;
  newhope_keygen(M_A, st);
}

void nike_resp(unsigned char *M_B, unsigned char *K_B, const unsigned char *M_A, const nike_params *p){
  (void)p;
  newhope_sharedb(K_B, M_B, M_A);
}

void nike_derive(unsigned char *K_A, const nike_state *st, const unsigned char *M_A, const unsigned char *M_B){
  (void)M_A;
  newhope_shareda(K_A, st, M_B);
}
