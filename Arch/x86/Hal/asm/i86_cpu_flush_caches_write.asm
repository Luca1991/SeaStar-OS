[BITS 32]
[GLOBAL i86_cpu_flush_caches_write]


i86_cpu_flush_caches_write:
    cli
    wbinvd
    sti
    ret


