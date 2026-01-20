/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/toolchain.h>
#include <zephyr/linker/sections.h>

_ASM_FILE_PROLOGUE

GTEXT(__start)
GTEXT(z_arm_reset)
GTEXT(z_arm_undef_instruction)
GTEXT(z_arm_svc)
GTEXT(z_arm_prefetch_abort)
GTEXT(z_arm_data_abort)
GTEXT(arch_irq_handle)
GTEXT(z_arm_fiq)

SECTION_SUBSEC_FUNC(vectors,_vector_table,_vector_table)

    ldr pc, =z_arm_reset
    ldr pc, =z_arm_undef_instruction
    ldr pc, =z_arm_svc
    ldr pc, =z_arm_prefetch_abort
    ldr pc, =z_arm_data_abort
    nop
    ldr pc, =arch_irq_handle
    ldr pc, =z_arm_fiq

GTEXT(z_arm_reset)
SECTION_FUNC(TEXT, z_arm_reset)
    /* Disable interrupts */
    cpsid if

    /* Set up stack pointers for different modes */

    /* FIQ mode stack */
    cps #0x11    /* FIQ mode */
    ldr sp, =z_fiq_stack

    /* IRQ mode stack */
    cps #0x12    /* IRQ mode */
    ldr sp, =z_irq_stack

    /* Abort mode stack */
    cps #0x17    /* Abort mode */
    ldr sp, =z_abt_stack

    /* Undefined mode stack */
    cps #0x1B    /* Undefined mode */
    ldr sp, =z_und_stack

    /* System mode stack */
    cps #0x1F    /* System mode */
    ldr sp, =z_sys_stack

    /* SVC mode stack (main stack) */
    cps #0x13    /* SVC mode */
    ldr sp, =z_main_stack

    /* Clear BSS */
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
bss_loop:
    cmp r0, r1
    bge bss_done
    str r2, [r0], #4
    b bss_loop
bss_done:

    /* Copy data section if needed */
    ldr r0, =__data_load_start
    ldr r1, =__data_start
    ldr r2, =__data_end
    cmp r1, r2
    beq data_done
data_loop:
    cmp r1, r2
    bge data_done
    ldr r3, [r0], #4
    str r3, [r1], #4
    b data_loop
data_done:

    /* Jump to kernel */
    bl z_cstart

    /* Should never get here */
hang:
    b hang

/* Default handlers */
GTEXT(z_arm_undef_instruction)
SECTION_FUNC(TEXT, z_arm_undef_instruction)
    b z_arm_undef_instruction

GTEXT(z_arm_svc)
SECTION_FUNC(TEXT, z_arm_svc)
    b z_arm_svc

GTEXT(z_arm_prefetch_abort)
SECTION_FUNC(TEXT, z_arm_prefetch_abort)
    b z_arm_prefetch_abort

GTEXT(z_arm_data_abort)
SECTION_FUNC(TEXT, z_arm_data_abort)
    b z_arm_data_abort

GTEXT(z_arm_fiq)
SECTION_FUNC(TEXT, z_arm_fiq)
    b z_arm_fiq

/* Stack definitions */
.section .stacks, "aw", %nobits

.align 3
z_irq_stack_base:
    .space 2048
z_irq_stack:

.align 3
z_fiq_stack_base:
    .space 2048
z_fiq_stack:

.align 3
z_abt_stack_base:
    .space 2048
z_abt_stack:

.align 3
z_und_stack_base:
    .space 2048
z_und_stack:

.align 3
z_sys_stack_base:
    .space 2048
z_sys_stack:

.align 3
z_main_stack_base:
    .space CONFIG_MAIN_STACK_SIZE
z_main_stack:
