[BITS 32]
[GLOBAL i86_cpu_flush_caches]

i86_cpu_flush_caches:
    cli
    invd
    sti
    ret
