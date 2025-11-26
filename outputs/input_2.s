.data
print_fmt_int: .string "%ld\n"
.text
.global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movq $0, -8(%rbp)
    movq $0, -16(%rbp)
    movq $0, -24(%rbp)
    movq $0, -32(%rbp)
    movq $0, %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq $100, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
while_0_start:
    movq -24(%rbp), %rax
    pushq %rax
    movq $0, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setg %al
    movzbq %al, %rax
    cmpq $0, %rax
    je while_0_end
    movq -32(%rbp), %rax
    pushq %rax
    movq $5, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setg %al
    movzbq %al, %rax
    cmpq $0, %rax
    je if_0_else
    movq $0, %rax
    movq %rax, -24(%rbp)
    jmp if_0_end
if_0_else:
if_0_end:
    movq $0, %rax
    movq %rax, -40(%rbp)
for_0_start:
    movq -40(%rbp), %rax
    pushq %rax
    movq $3, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je for_0_end
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setg %al
    movzbq %al, %rax
    cmpq $0, %rax
    je ternary_0_false
    movq -16(%rbp), %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    jmp ternary_0_end
ternary_0_false:
    movq -16(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
ternary_0_end:
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    sete %al
    movzbq %al, %rax
    cmpq $0, %rax
    je if_1_else
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -24(%rbp), %rax
    pushq %rax
    movq $20, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, -24(%rbp)
    jmp if_1_end
if_1_else:
if_1_end:
    movq -40(%rbp), %rax
    incq %rax
    movq %rax, -40(%rbp)
    jmp for_0_start
for_0_end:
    movq -32(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -24(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -32(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax
    pushq %rax
    movq $10, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, -24(%rbp)
    jmp while_0_start
while_0_end:
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
