#include "KEX_AlgorithmInstance.h"
#include <string.h>
#include "src/nike.h"

#if 192 == 128
#define PROFILE NIKE_128
#elif 192 == 192
#define PROFILE NIKE_192
#elif 192 == 256
#define PROFILE NIKE_256
#elif 192 == 384
#define PROFILE NIKE_384
#else
#define PROFILE NIKE_512
#endif

typedef struct {
    nike_state st;
    unsigned char ma[2848];
    unsigned long long ma_len;
} state_a_t;

static const nike_params *params_ref(void){ return &PROFILE; }
unsigned long long kex_get_passes_num(){ return 2; }
unsigned long long kex_get_pk_len_bytes(){ return 0; }
unsigned long long kex_get_sk_len_bytes(){ return 0; }
unsigned long long kex_get_sta_len_bytes(){ return sizeof(state_a_t); }
unsigned long long kex_get_stb_len_bytes(){ return 0; }
unsigned long long kex_get_ss_len_bytes(){ return params_ref()->ss_bytes; }
unsigned long long kex_get_total_msg_len_bytes(){ return nike_mb_bytes(params_ref()); }
int kex_init_a(unsigned char *pka, unsigned long long *pka_len_bytes,unsigned char *ska, unsigned long long *ska_len_bytes,unsigned char *sta, unsigned long long *sta_len_bytes){(void)pka;(void)ska;*pka_len_bytes=0;*ska_len_bytes=0;memset(sta,0,sizeof(state_a_t));*sta_len_bytes=sizeof(state_a_t);return 0;}
int kex_init_b(unsigned char *pkb, unsigned long long *pkb_len_bytes,unsigned char *skb, unsigned long long *skb_len_bytes,unsigned char *stb, unsigned long long *stb_len_bytes){(void)pkb;(void)skb;(void)stb;*pkb_len_bytes=0;*skb_len_bytes=0;*stb_len_bytes=0;return 0;}
int kex_generate_pass1_msg_a(unsigned char *ska, unsigned long long ska_len_bytes,unsigned char *pkb, unsigned long long pkb_len_bytes,unsigned char *sta, unsigned long long *sta_len_bytes,unsigned char *m1, unsigned long long *m1_len_bytes){(void)ska;(void)ska_len_bytes;(void)pkb;(void)pkb_len_bytes;state_a_t *s=(state_a_t*)sta;size_t ml=nike_ma_bytes(params_ref());nike_init(&s->st,m1,params_ref());memcpy(s->ma,m1,ml);s->ma_len=ml;*m1_len_bytes=ml;*sta_len_bytes=sizeof(state_a_t);return 0;}
int kex_generate_pass2_msg_b(unsigned char *skb, unsigned long long skb_len_bytes,unsigned char *pka, unsigned long long pka_len_bytes,unsigned char *m1, unsigned long long m1_len_bytes,unsigned char *stb, unsigned long long *stb_len_bytes,unsigned char *m2, unsigned long long *m2_len_bytes){(void)skb;(void)skb_len_bytes;(void)pka;(void)pka_len_bytes;(void)stb;(void)stb_len_bytes;if(m1_len_bytes!=nike_ma_bytes(params_ref())) return -1;nike_resp(m2,NULL,m1,params_ref());*m2_len_bytes=nike_mb_bytes(params_ref());return 1;}
int kex_generate_pass3_msg_a(unsigned char *ska, unsigned long long ska_len_bytes,unsigned char *pkb, unsigned long long pkb_len_bytes,unsigned char *m2, unsigned long long m2_len_bytes,unsigned char *sta, unsigned long long *sta_len_bytes,unsigned char *m3, unsigned long long *m3_len_bytes){(void)ska;(void)ska_len_bytes;(void)pkb;(void)pkb_len_bytes;(void)m2;(void)m2_len_bytes;(void)sta;(void)sta_len_bytes;(void)m3;*m3_len_bytes=0;return -1;}
int kex_derive_ss_a(unsigned char *ska, unsigned long long ska_len_bytes,unsigned char *pkb, unsigned long long pkb_len_bytes,unsigned char *mb, unsigned long long mb_len_bytes,unsigned char *sta, unsigned long long sta_len_bytes,unsigned char *ssa, unsigned long long *ssa_len_bytes){(void)ska;(void)ska_len_bytes;(void)pkb;(void)pkb_len_bytes;(void)sta_len_bytes;state_a_t *s=(state_a_t*)sta;if(mb_len_bytes!=nike_mb_bytes(params_ref())) return -1;nike_derive(ssa,&s->st,s->ma,mb);*ssa_len_bytes=params_ref()->ss_bytes;return 0;}
int kex_derive_ss_b(unsigned char *skb, unsigned long long skb_len_bytes,unsigned char *pka, unsigned long long pka_len_bytes,unsigned char *ma, unsigned long long ma_len_bytes,unsigned char *stb, unsigned long long stb_len_bytes,unsigned char *ssb, unsigned long long *ssb_len_bytes){(void)skb;(void)skb_len_bytes;(void)pka;(void)pka_len_bytes;(void)stb;(void)stb_len_bytes;if(ma_len_bytes!=nike_ma_bytes(params_ref())) return -1;nike_resp(NULL,ssb,ma,params_ref());*ssb_len_bytes=params_ref()->ss_bytes;return 0;}
