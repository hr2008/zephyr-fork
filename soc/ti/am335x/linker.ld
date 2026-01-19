OUTPUT_FORMAT("elf32-littlearm")
ENTRY(z_arm_reset)

MEMORY
{
    DDR_RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS
{
    /* ==============================================================
     * Image start
     * ============================================================== */
    . = ORIGIN(DDR_RAM);
    _image_ram_start = .;
    z_mapped_start = .;

    /* ==============================================================
     * Text + vectors
     * ============================================================== */
    .text ALIGN(4) :
    {
        __text_region_start = .;

        _vector_start = .;
        KEEP(*(.exc_vector_table))
        KEEP(*(.exc_vector_table.*))
        KEEP(*(.vectors))
        _vector_end = .;

        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)

        __text_region_end = .;
    } > DDR_RAM

    /* ==============================================================
     * Read-only data
     * ============================================================== */
    .rodata ALIGN(4) :
    {
        __rodata_region_start = .;

        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)

        __rodata_region_end = .;
    } > DDR_RAM

    /* ==============================================================
     * Init arrays
     * ============================================================== */
    .init_array ALIGN(4) :
    {
        __zephyr_init_array_start = .;
        KEEP(*(.init_array*))
        KEEP(*(.ctors*))
        __zephyr_init_array_end = .;
    } > DDR_RAM

    /* ==============================================================
     * Init levels (ALL REQUIRED START SYMBOLS)
     * ============================================================== */
    .initlevel ALIGN(4) :
    {
        __init_start = .;

        __init_EARLY_start = .;
        KEEP(*(.z_init_EARLY_*))

        __init_PRE_KERNEL_1_start = .;
        KEEP(*(.z_init_PRE_KERNEL_1_*))

        __init_PRE_KERNEL_2_start = .;
        KEEP(*(.z_init_PRE_KERNEL_2_*))

        __init_POST_KERNEL_start = .;
        KEEP(*(.z_init_POST_KERNEL_*))

        __init_APPLICATION_start = .;
        KEEP(*(.z_init_APPLICATION_*))

        __init_SMP_start = .;
        KEEP(*(.z_init_SMP_*))

        __init_end = .;
    } > DDR_RAM

    /* ==============================================================
     * TLS (unchanged, mandatory)
     * ============================================================== */
    .tdata ALIGN(4) :
    {
        *(.tdata .tdata.* .gnu.linkonce.td.*)
    } > DDR_RAM

    .tbss ALIGN(4) (NOLOAD) :
    {
        *(.tbss .tbss.* .gnu.linkonce.tb.* .tcommon)
    } > DDR_RAM

    PROVIDE(__tdata_start = ADDR(.tdata));
    PROVIDE(__tdata_align = ALIGNOF(.tdata));
    PROVIDE(__tdata_size =
        (SIZEOF(.tdata) + __tdata_align - 1) & ~(__tdata_align - 1));

    PROVIDE(__tbss_align = ALIGNOF(.tbss));
    PROVIDE(__tbss_start = ADDR(.tbss));
    PROVIDE(__tbss_size =
        (SIZEOF(.tbss) + __tbss_align - 1) & ~(__tbss_align - 1));

    PROVIDE(__tls_start = __tdata_start);
    PROVIDE(__tls_end   = __tbss_start + __tbss_size);
    PROVIDE(__tls_size  = __tls_end - __tls_start);

    /* ==============================================================
     * Data
     * ============================================================== */
    .data ALIGN(4) :
    {
        __data_start = .;
        *(.data*)
        __data_end = .;
    } > DDR_RAM

    /* ==============================================================
     * Static thread data
     * ============================================================== */
    .static_thread_data_area ALIGN(4) :
    {
        __static_thread_data_list_start = .;
        KEEP(*(SORT_BY_NAME(.__static_thread_data.static.*)))
        __static_thread_data_list_end = .;
    } > DDR_RAM

    /* ==============================================================
     * Kernel heap list
     * ============================================================== */
    .k_heap_area ALIGN(4) :
    {
        _k_heap_list_start = .;
        KEEP(*(SORT_BY_NAME(._k_heap.static.*)))
        _k_heap_list_end = .;
    } > DDR_RAM

    /* ==============================================================
     * BSS
     * ============================================================== */
    .bss ALIGN(4) (NOLOAD) :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > DDR_RAM

    /* ==============================================================
     * End markers
     * ============================================================== */
    z_mapped_end = .;
    _image_ram_end = .;

    /* ==============================================================
     * Interrupt list
     * ============================================================== */
    .intList ALIGN(4) :
    {
        KEEP(*(.irq_info*))
        KEEP(*(.intList*))
    } > DDR_RAM

    /DISCARD/ :
    {
        *(.note.GNU-stack)
        *(.comment)
    }
}
