#include "KEX_AlgorithmInstance.h"
#include <string.h>
#include "src/newhope.h"
#include "src/params.h"

typedef struct { poly sk; unsigned char ma[NEWHOPE_SENDABYTES]; } state_a_t;
unsigned long long kex_get_passes_num(){ return 2; }
unsigned long long kex_get_pk_len_bytes(){ return 0; }
unsigned long long kex_get_sk_len_bytes(){ return 0; }
unsigned long long kex_get_sta_len_bytes(){ return sizeof(state_a_t); }
unsigned long long kex_get_stb_len_bytes(){ return 0; }
unsigned long long kex_get_ss_len_bytes(){ return 32; }
unsigned long long kex_get_total_msg_len_bytes(){ return NEWHOPE_SENDABYTES + NEWHOPE_SENDBBYTES; }
int kex_init_a(unsigned char *pka,unsigned long long *pka_len,unsigned char *ska,unsigned long long *ska_len,unsigned char *sta,unsigned long long *sta_len){(void)pka;(void)ska;*pka_len=0;*ska_len=0;memset(sta,0,sizeof(state_a_t));*sta_len=sizeof(state_a_t);return 0;}
int kex_init_b(unsigned char *pkb,unsigned long long *pkb_len,unsigned char *skb,unsigned long long *skb_len,unsigned char *stb,unsigned long long *stb_len){(void)pkb;(void)skb;(void)stb;*pkb_len=0;*skb_len=0;*stb_len=0;return 0;}
int kex_generate_pass1_msg_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *sta,unsigned long long *sta_len,unsigned char *m1,unsigned long long *m1_len){(void)ska;(void)ska_len;(void)pkb;(void)pkb_len;state_a_t *s=(state_a_t*)sta;newhope_keygen(m1,&s->sk);memcpy(s->ma,m1,NEWHOPE_SENDABYTES);*m1_len=NEWHOPE_SENDABYTES;*sta_len=sizeof(state_a_t);return 0;}
int kex_generate_pass2_msg_b(unsigned char *skb,unsigned long long skb_len,unsigned char *pka,unsigned long long pka_len,unsigned char *m1,unsigned long long m1_len,unsigned char *stb,unsigned long long *stb_len,unsigned char *m2,unsigned long long *m2_len){(void)skb;(void)skb_len;(void)pka;(void)pka_len;(void)stb;(void)stb_len;if(m1_len!=NEWHOPE_SENDABYTES) return -1;unsigned char kb[32];newhope_sharedb(kb,m2,m1);*m2_len=NEWHOPE_SENDBBYTES;return 1;}
int kex_generate_pass3_msg_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *m2,unsigned long long m2_len,unsigned char *sta,unsigned long long *sta_len,unsigned char *m3,unsigned long long *m3_len){(void)ska;(void)ska_len;(void)pkb;(void)pkb_len;(void)m2;(void)m2_len;(void)sta;(void)sta_len;(void)m3;*m3_len=0;return -1;}
int kex_derive_ss_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *mb,unsigned long long mb_len,unsigned char *sta,unsigned long long sta_len,unsigned char *ssa,unsigned long long *ssa_len){(void)ska;(void)ska_len;(void)pkb;(void)pkb_len;(void)sta_len;state_a_t *s=(state_a_t*)sta;if(mb_len!=NEWHOPE_SENDBBYTES) return -1;newhope_shareda(ssa,&s->sk,mb);*ssa_len=32;return 0;}
int kex_derive_ss_b(unsigned char *skb,unsigned long long skb_len,unsigned char *pka,unsigned long long pka_len,unsigned char *ma,unsigned long long ma_len,unsigned char *stb,unsigned long long stb_len,unsigned char *ssb,unsigned long long *ssb_len){(void)skb;(void)skb_len;(void)pka;(void)pka_len;(void)stb;(void)stb_len;if(ma_len!=NEWHOPE_SENDABYTES) return -1;unsigned char mb[NEWHOPE_SENDBBYTES];newhope_sharedb(ssb,mb,ma);*ssb_len=32;return 0;}
