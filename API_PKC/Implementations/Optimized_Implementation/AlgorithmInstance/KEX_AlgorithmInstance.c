#include "KEX_AlgorithmInstance.h"
#include "drng.h"
#include "../NIKE/nike.h"
#include <string.h>

#define KEX_MAX_MA_BYTES 2848
#define KEX_MAX_SS_BYTES 64

typedef struct {
    nike_state st;
    unsigned char ma[KEX_MAX_MA_BYTES];
    unsigned long long ma_len;
} kex_sta;

typedef struct {
    unsigned char bob_ss[KEX_MAX_SS_BYTES];
    unsigned long long bob_ss_len;
    int bob_ss_ready;
} kex_stb;

extern DRNG_ctx drng_algorithm;

static const nike_params* params(void){
#if NIKE_PROFILE==128
    return &NIKE_128_PARAMS;
#elif NIKE_PROFILE==192
    return &NIKE_192_PARAMS;
#elif NIKE_PROFILE==256
    return &NIKE_256_PARAMS;
#elif NIKE_PROFILE==384
    return &NIKE_384_PARAMS;
#else
    return &NIKE_512_PARAMS;
#endif
}

unsigned long long kex_get_passes_num(){ return 3; }
unsigned long long kex_get_pk_len_bytes(){ return 0; }
unsigned long long kex_get_sk_len_bytes(){ return 0; }
unsigned long long kex_get_sta_len_bytes(){ return sizeof(kex_sta); }
unsigned long long kex_get_stb_len_bytes(){ return sizeof(kex_stb); }
unsigned long long kex_get_ss_len_bytes(){ return params()->ss_bytes; }
unsigned long long kex_get_total_msg_len_bytes(){ return params()->ma_bytes + params()->mb_bytes; }

int kex_init_a(unsigned char *pka,unsigned long long *pka_len,unsigned char *ska,unsigned long long *ska_len,unsigned char *sta,unsigned long long *sta_len){
    (void)pka; (void)ska; (void)sta;
    *pka_len = 0; *ska_len = 0; *sta_len = 0;
    return 0;
}

int kex_init_b(unsigned char *pkb,unsigned long long *pkb_len,unsigned char *skb,unsigned long long *skb_len,unsigned char *stb,unsigned long long *stb_len){
    (void)pkb; (void)skb;
    *pkb_len = 0; *skb_len = 0;
    if (stb && *stb_len >= sizeof(kex_stb)) {
        kex_stb tmp; memset(&tmp, 0, sizeof(tmp)); memcpy(stb, &tmp, sizeof(tmp));
        *stb_len = sizeof(tmp);
    } else if (stb_len) {
        *stb_len = sizeof(kex_stb);
    }
    return 0;
}

int kex_generate_pass1_msg_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *sta,unsigned long long *sta_len,unsigned char *m1,unsigned long long *m1_len){
    (void)ska; (void)ska_len; (void)pkb; (void)pkb_len;
    const nike_params *p = params();
    if (p->ma_bytes > KEX_MAX_MA_BYTES) return -1;
    kex_sta loc; memset(&loc, 0, sizeof(loc));
    nike_init(&loc.st, m1, p);
    memcpy(loc.ma, m1, p->ma_bytes);
    loc.ma_len = p->ma_bytes;
    memcpy(sta, &loc, sizeof(loc));
    *sta_len = sizeof(loc);
    *m1_len = p->ma_bytes;
    return 0;
}

int kex_generate_pass2_msg_b(unsigned char *skb,unsigned long long skb_len,unsigned char *pka,unsigned long long pka_len,unsigned char *m1,unsigned long long m1_len,unsigned char *stb,unsigned long long *stb_len,unsigned char *m2,unsigned long long *m2_len){
    (void)skb; (void)skb_len; (void)pka; (void)pka_len;
    const nike_params *p = params();
    if (m1_len != p->ma_bytes || p->ss_bytes > KEX_MAX_SS_BYTES) return -1;

    unsigned char kb[KEX_MAX_SS_BYTES];
    memset(kb, 0, sizeof(kb));
    nike_resp(m2, kb, m1, p);

    if (stb == NULL || stb_len == NULL || *stb_len < sizeof(kex_stb)) return -1;
    kex_stb bobctx; memset(&bobctx, 0, sizeof(bobctx));
    memcpy(bobctx.bob_ss, kb, p->ss_bytes);
    bobctx.bob_ss_len = p->ss_bytes;
    bobctx.bob_ss_ready = 1;
    memcpy(stb, &bobctx, sizeof(bobctx));
    *stb_len = sizeof(bobctx);

    *m2_len = p->mb_bytes;
    return 0;
}

int kex_generate_pass3_msg_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *m2,unsigned long long m2_len,unsigned char *sta,unsigned long long *sta_len,unsigned char *m3,unsigned long long *m3_len){
    (void)ska; (void)ska_len; (void)pkb; (void)pkb_len; (void)m2; (void)m2_len; (void)sta; (void)sta_len; (void)m3;
    *m3_len = 0;
    return 1;
}

int kex_derive_ss_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *mb,unsigned long long mb_len,unsigned char *sta,unsigned long long sta_len,unsigned char *ssa,unsigned long long *ssa_len){
    (void)ska; (void)ska_len; (void)pkb; (void)pkb_len;
    const nike_params *p = params();
    if (mb_len != p->mb_bytes || sta_len < sizeof(kex_sta)) return -1;
    kex_sta loc; memcpy(&loc, sta, sizeof(loc));
    nike_derive(ssa, &loc.st, loc.ma, mb);
    *ssa_len = p->ss_bytes;
    return 0;
}

int kex_derive_ss_b(unsigned char *skb,unsigned long long skb_len,unsigned char *pka,unsigned long long pka_len,unsigned char *ma,unsigned long long ma_len,unsigned char *stb,unsigned long long stb_len,unsigned char *ssb,unsigned long long *ssb_len){
    (void)skb; (void)skb_len; (void)pka; (void)pka_len; (void)ma; (void)ma_len;
    if (stb == NULL || ssb == NULL || ssb_len == NULL || stb_len < sizeof(kex_stb)) return -1;
    kex_stb bobctx; memcpy(&bobctx, stb, sizeof(bobctx));
    if (!bobctx.bob_ss_ready || bobctx.bob_ss_len > KEX_MAX_SS_BYTES) return -1;
    memcpy(ssb, bobctx.bob_ss, bobctx.bob_ss_len);
    *ssb_len = bobctx.bob_ss_len;
    bobctx.bob_ss_ready = 0;
    memcpy(stb, &bobctx, sizeof(bobctx));
    return 0;
}

void randombytes(unsigned char *x, unsigned long long xlen){
    get_random_number(&drng_algorithm, x, xlen * 8);
}
