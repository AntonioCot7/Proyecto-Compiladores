.data
print_fmt_int: .string "%ld\n"
.text
.global main
fib:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    subq $16, %rsp
    movq -8(%rbp), %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je ternary_0_false
    movq -8(%rbp), %rax
    jmp ternary_0_end
ternary_0_false:
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    pushq %rax
    popq %rdi
    movl $0, %eax
    call fib
    pushq %rax
    movq -8(%rbp), %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    pushq %rax
    popq %rdi
    movl $0, %eax
    call fib
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
ternary_0_end:
    jmp .end_fib
.end_fib:
    leave
    ret
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, -8(%rbp)
    movq $0, %rax
    movq %rax, -8(%rbp)
while_0_start:
    movq -8(%rbp), %rax
    pushq %rax
    movq $10, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je while_0_end
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -8(%rbp), %rax
    pushq %rax
    popq %rdi
    movl $0, %eax
    call fib
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -8(%rbp)
    jmp while_0_start
while_0_end:
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
