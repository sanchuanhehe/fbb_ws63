# WS63 factory TX alignment workaround

The workaround is limited to `ws63-liteos-app` and the audited Controller archive
SHA-256 `bfedcd51bf15e4ccda299ad09ccc09d212ef1e150d98fc80dde099c409c892e2`.
CMake rejects a different archive. It does not modify the vendor archive or ROM.

Observed baseline: the factory allocator returns an EM block+2 pointer, e.g.
0x5af6. PRBS9 is correct there, but FT2/Polar3/4 RF decoding starts with the two
preceding bytes and loses the last two payload bytes. Uncoded TX is correct.
The original hardware evidence is maintained in the sparklink-hw repository:
`verification/runs/20261115-ws63-fresh-sdk.json`.

The final `evt_task_single_task_start` call is wrapped. Other tasks and non-FT2/
non-Polar3/4 factory requests are passed through. Before publication, the wrapper
checks the environment, allocated buffer layout, final SED→TXPD reference,
rate/coding, length and room for a forward alignment move. It moves data backwards
within the already owned block and changes only TXPD+4 to the aligned source.
The allocator node, length and preceding headroom are unchanged. The original
caller still handles submission failure, stop and freeing the same nodes.
No new scheduler, transaction API, RF owner or receive workaround is added.

Host regression uses the actual C move/guards, not a rewritten model:

```sh
cc -std=c11 -O2 -Wall -Wextra -Werror tests/ws63_factory_tx/test_align4.c -o /tmp/ws63-align4-test
/tmp/ws63-align4-test
```

Clang ASan/UBSan is also applicable. LeakSanitizer cannot run under this task's
ptrace-based tool environment; it was disabled separately, without disabling
address/undefined-behavior checks. This function allocates no heap memory.

The tests cover lengths0..1528, overlap, multiple blocks, already aligned input,
unchanged allocator fields/canaries and rejection without any write. These are
memory tests, not proof of hardware support for every tested length.

Full SDK build from `src/` uses the existing build entry. Hardware qualification,
raw IQ, input hashes and the final cause/scope report remain in sparklink-hw's
existing capture-tools OpenSpec and evidence records. A successful build or host
test alone is not RF qualification. All unrelated production PHY/SLE goals stay
separate. The startup Flash warning0x80001341 is not fixed by this workaround.

The first matched RF run on2026-11-16 has passed: coded30/30 CRC-qualified
frames equal the complete255-byte PRBS9, with uncoded control also passing.
Final TXPD moves0x5af6→0x5af8 while the allocator node stays0x5af6.
The same frozen SDK/model/RF setup is being checked across the24 original coded
combinations; do not infer their acceptance from this first representative run.
