#include <stdint.h>
#include <string.h>

// Символы из линкер‑скрипта (sections.lds через spifi.ld)
extern uint8_t __BSS_START__;
extern uint8_t __BSS_END__;
extern uint8_t __SBSS_START__;
extern uint8_t __SBSS_END__;
extern uint8_t __DATA_START__;
extern const uint8_t __DATA_IMAGE_START__;
extern const uint8_t __DATA_IMAGE_END__;
extern uint8_t __global_pointer$;
extern uint8_t __C_STACK_TOP__;

// Точка входа (определена как ENTRY(_start) в spifi.ld)
void _start(void) __attribute__((noreturn));
void _Exit(int exit_code) __attribute__((noreturn, noinline));
extern int main(void);

// Безопасный обработчик исключений (бесконечный цикл, чтобы не падать в unknown)
void trap_handler(void) __attribute__((weak));
void trap_handler(void) {
    while(1) {}
}

void _start(void) {
    // 1. Настройка глобального указателя и стека
    asm volatile(
        ".option push\n\t"
        ".option norelax\n\t"
        "lui   gp, %%hi(__global_pointer$)\n\t"
        "addi  gp, gp, %%lo(__global_pointer$)\n\t"
        "lui   sp, %%hi(__C_STACK_TOP__)\n\t"
        "addi  sp, sp, %%lo(__C_STACK_TOP__)\n\t"
        ".option pop\n\t"
        : : : "memory"
    );

    // 2. Направить вектор прерываний (mtvec) на наш обработчик
    asm volatile ("csrw mtvec, %0" :: "r"(&trap_handler));

    // 3. Очистить BSS (глобальные переменные без начальных значений)
    memset(&__SBSS_START__, 0, &__SBSS_END__ - &__SBSS_START__);
    memset(&__BSS_START__, 0, &__BSS_END__ - &__BSS_START__);

    // 4. Скопировать .data/.sdata/.srodata из flash в RAM
    size_t data_len = &__DATA_IMAGE_END__ - &__DATA_IMAGE_START__;
    memcpy(&__DATA_START__, &__DATA_IMAGE_START__, data_len);

    // 5. Небольшая задержка, чтобы отладчик успел «зацепиться»
    for (volatile int i = 0; i < 100000; i++) { asm("nop"); }

    // 6. Переход к main
    int rc = main();
    _Exit(rc);
}

void _Exit(int exit_code) {
    (void)exit_code;
    while (1) {
        __asm__ volatile("wfi");
    }
}