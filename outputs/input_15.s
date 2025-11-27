.data
print_fmt_int: .string "%ld\n"
.text
.global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, %rax
    movq %rax, -8(%rbp)
for_0_start:
    movq -8(%rbp), %rax
    pushq %rax
    movq $5, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je for_0_end
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -8(%rbp), %rax
    incq %rax
    movq %rax, -8(%rbp)
    jmp for_0_start
for_0_end:
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
