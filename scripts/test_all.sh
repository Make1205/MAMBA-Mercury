#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
OUT_TXT="$BUILD_DIR/mamba_sign_test_all.txt"
OUT_CSV="$BUILD_DIR/mamba_sign_test_all.csv"
mkdir -p "$BUILD_DIR"

PROFILES=("NIKE-128" "NIKE-192" "NIKE-256" "NIKE-384" "NIKE-512")
ITERS="${BENCH_ITERS:-1000}"

echo "Running one-shot NIKE correctness/speed runner"
echo "BENCH_ITERS=$ITERS"

# Build ref test binary
make -C "$ROOT_DIR/ref" test/test_nike_ref >/dev/null
REF_LOG="$BUILD_DIR/ref_test_nike_ref.log"
"$ROOT_DIR/ref/test/test_nike_ref" > "$REF_LOG"

# Detect AVX2 NIKE support
AVX2_SUPPORTED=0
if [ -f "$ROOT_DIR/avx2/test/test_nike_ref" ]; then
  AVX2_SUPPORTED=1
fi

{
  echo "MAMBA-NIKE one-shot test report"
  echo "iterations=$ITERS"
  echo "timing_source=cpucycles(if emitted by binary); otherwise N/A"
  echo
  echo "profile,implementation,correctness,init_cycles,resp_cycles,derive_cycles,MA_bytes,MB_bytes,total_bytes,ss_bytes"
} > "$OUT_TXT"

echo "profile,implementation,correctness,init_cycles,resp_cycles,derive_cycles,MA_bytes,MB_bytes,total_bytes,ss_bytes" > "$OUT_CSV"

get_ref_profile_block() {
  local p="$1"
  awk -v p="$p" '
    $0 ~ "backend=ntt " p {inblk=1; next}
    inblk && $0 ~ /^backend=/ {exit}
    inblk {print}
  ' "$REF_LOG"
}

extract_mode3() { grep -E "mode3 success" | tail -n1 | awk '{print $3}'; }
extract_sizes() { grep -E "ss_bytes=.*\|M_A\|=" | tail -n1; }

for p in "${PROFILES[@]}"; do
  blk="$(get_ref_profile_block "$p")"
  corr="$(printf "%s\n" "$blk" | extract_mode3)"
  [ -z "$corr" ] && corr="N/A"
  sizes="$(printf "%s\n" "$blk" | extract_sizes)"
  ma="N/A"; mb="N/A"; total="N/A"; ss="N/A"
  if [ -n "$sizes" ]; then
    ss="$(echo "$sizes" | sed -E 's/.*ss_bytes=([0-9]+).*/\1/')"
    ma="$(echo "$sizes" | sed -E 's/.*\|M_A\|=([0-9]+).*/\1/')"
    mb="$(echo "$sizes" | sed -E 's/.*\|M_B\|=([0-9]+).*/\1/')"
    total="$(echo "$sizes" | sed -E 's/.*total=([0-9]+).*/\1/')"
  fi
  init="N/A"; resp="N/A"; derive="N/A"
  echo "$p,ref,$corr,$init,$resp,$derive,$ma,$mb,$total,$ss" | tee -a "$OUT_TXT" >> "$OUT_CSV"

done

for p in "${PROFILES[@]}"; do
  if [ "$AVX2_SUPPORTED" -eq 1 ]; then
    corr="TODO"
  else
    corr="not implemented"
  fi
  echo "$p,avx2,$corr,N/A,N/A,N/A,N/A,N/A,N/A,N/A" | tee -a "$OUT_TXT" >> "$OUT_CSV"
done

echo >> "$OUT_TXT"
if [ "$AVX2_SUPPORTED" -eq 0 ]; then
  echo "avx2: not implemented for NIKE profiles (TODO)" | tee -a "$OUT_TXT"
fi

echo "Results written to:"
echo "  $OUT_TXT"
echo "  $OUT_CSV"
