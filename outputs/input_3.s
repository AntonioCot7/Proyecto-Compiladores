.data
print_fmt_int: .string "%ld\n"
.text
.global main
withdraw:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, %rax
    movq 0(%rax), %rcx
    movq %rcx, -8(%rbp)
    movq -8(%rax), %rcx
    movq %rcx, -16(%rbp)
    movq %rsi, -24(%rbp)
    subq $48, %rsp
    movq $0, -32(%rbp)
    movq -16(%rbp), %rax
    pushq %rax
    movq -24(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setge %al
    movzbq %al, %rax
    cmpq $0, %rax
    je ternary_0_false
    movq $1, %rax
    jmp ternary_0_end
ternary_0_false:
    movq $0, %rax
ternary_0_end:
    movq %rax, -32(%rbp)
    movq -32(%rbp), %rax
    cmpq $0, %rax
    je if_0_else
    movq -16(%rbp), %rax
    pushq %rax
    movq -24(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    jmp if_0_end
if_0_else:
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
if_0_end:
    movq -8(%rbp), %rax
    movq -8(%rbp), %rax
    movq -16(%rbp), %rdx
    jmp .end_withdraw
.end_withdraw:
    leave
    ret
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $0, -8(%rbp)
    movq $0, -16(%rbp)
    movq $1, %rax
    movq %rax, -8(%rbp)
    movq $1000, %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    leaq -8(%rbp), %rax
    pushq %rax
    movq $300, %rax
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call withdraw
    movq %rax, -8(%rbp)
    movq %rdx, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    leaq -8(%rbp), %rax
    pushq %rax
    movq $800, %rax
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call withdraw
    movq %rax, -8(%rbp)
    movq %rdx, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
