#!/usr/bin/env bash
set -u
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
OUT_TXT="$BUILD_DIR/mamba_nike_test_all.txt"
OUT_CSV="$BUILD_DIR/mamba_nike_test_all.csv"
ITERS="${BENCH_ITERS:-1000}"
mkdir -p "$BUILD_DIR"

make -C "$ROOT_DIR/ref" test/test_nike_bench >/dev/null
REF_CSV="$BUILD_DIR/ref_nike_bench.csv"
"$ROOT_DIR/ref/test/test_nike_bench" "$ITERS" ref > "$REF_CSV"

AVX2_STATUS="ok"
AVX2_IMPL="avx2"
AVX2_CSV="$BUILD_DIR/avx2_nike_bench.csv"
if make -C "$ROOT_DIR/avx2" test/test_nike_avx2 >/dev/null 2>&1; then
  "$ROOT_DIR/avx2/test/test_nike_avx2" "$ITERS" > "$AVX2_CSV" || AVX2_STATUS="run_failed"
  AVX2_IMPL="avx2-mixed"
else
  AVX2_STATUS="build_failed"
fi

{
  echo "MAMBA-NIKE one-shot test report"
  echo "iterations=$ITERS"
  echo "cycle_stat=mean(cpucycles over iterations)"
  echo "profile,implementation,correctness,init_cycles,resp_cycles,derive_cycles,MA_bytes,MB_bytes,total_bytes,ss_bytes"
  tail -n +2 "$REF_CSV"
  if [ "$AVX2_STATUS" = "ok" ]; then
    tail -n +2 "$AVX2_CSV"
  else
    for p in NIKE-128 NIKE-192 NIKE-256 NIKE-384 NIKE-512; do
      echo "$p,avx2,$AVX2_STATUS,N/A,N/A,N/A,N/A,N/A,N/A,N/A"
    done
  fi
} > "$OUT_TXT"

{
  echo "profile,implementation,correctness,init_cycles,resp_cycles,derive_cycles,MA_bytes,MB_bytes,total_bytes,ss_bytes"
  tail -n +2 "$REF_CSV"
  if [ "$AVX2_STATUS" = "ok" ]; then
    tail -n +2 "$AVX2_CSV"
  else
    for p in NIKE-128 NIKE-192 NIKE-256 NIKE-384 NIKE-512; do
      echo "$p,avx2,$AVX2_STATUS,N/A,N/A,N/A,N/A,N/A,N/A,N/A"
    done
  fi
} > "$OUT_CSV"

echo "Running one-shot NIKE correctness/speed runner"
echo "BENCH_ITERS=$ITERS"
if [ "$AVX2_STATUS" != "ok" ]; then
  echo "avx2 status: $AVX2_STATUS"
else
  echo "avx2 status: $AVX2_IMPL"
fi
echo "Results written to:"
echo "  $OUT_TXT"
echo "  $OUT_CSV"
