#ifndef KEX_ALGORITHM_INSTANCE_H
#define KEX_ALGORITHM_INSTANCE_H
#define OUTPUT_BLANK_TEST_VECTORS 0
#ifndef NIKE_PROFILE
#define NIKE_PROFILE 128
#endif
#define STR2(x) #x
#define STR(x) STR2(x)
#define ALGORITHM_INSTANCE "NIKE-" STR(NIKE_PROFILE)
#ifdef __cplusplus
extern "C" {
#endif
unsigned long long kex_get_passes_num();
unsigned long long kex_get_pk_len_bytes();
unsigned long long kex_get_sk_len_bytes();
unsigned long long kex_get_sta_len_bytes();
unsigned long long kex_get_stb_len_bytes();
unsigned long long kex_get_ss_len_bytes();
unsigned long long kex_get_total_msg_len_bytes();
int kex_init_a(unsigned char*,unsigned long long*,unsigned char*,unsigned long long*,unsigned char*,unsigned long long*);
int kex_init_b(unsigned char*,unsigned long long*,unsigned char*,unsigned long long*,unsigned char*,unsigned long long*);
int kex_generate_pass1_msg_a(unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long*,unsigned char*,unsigned long long*);
int kex_generate_pass2_msg_b(unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long*,unsigned char*,unsigned long long*);
int kex_generate_pass3_msg_a(unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long*,unsigned char*,unsigned long long*);
int kex_derive_ss_a(unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long*);
int kex_derive_ss_b(unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long,unsigned char*,unsigned long long*);
#ifdef __cplusplus
}
#endif
#endif
