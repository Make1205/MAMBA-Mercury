import sys
r=open(sys.argv[1]).read().splitlines(); a=open(sys.argv[2]).read().splitlines()
for i,k in enumerate(['M1_Len','M2_Len','SS_Len','RNG_Calls','RNG_Lens']):
    rr=r[i] if i<4 else r[4]
    aa=a[i] if i<4 else a[4]
    if rr!=aa:
        print('mismatch',k,rr,aa);sys.exit(1)
for idx,name in [(5,'M1'),(6,'M2'),(7,'SS')]:
    if r[idx]!=a[idx]:
        n=min(len(r[idx]),len(a[idx])); p=next((j for j in range(0,n,2) if r[idx][j:j+2]!=a[idx][j:j+2]),None)
        print('mismatch',name,'ref_len',len(r[idx]),'avx2_len',len(a[idx]))
        print('ref_head64',r[idx][:64]); print('avx2_head64',a[idx][:64])
        if p is not None: print('first_byte_off',p//2)
        sys.exit(1)
print('PASS')
