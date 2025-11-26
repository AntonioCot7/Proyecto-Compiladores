.data
print_fmt_int: .string "%ld\n"
.text
.global main
ack:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    subq $32, %rsp
    movq -8(%rbp), %rax
    pushq %rax
    movq $0, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    sete %al
    movzbq %al, %rax
    cmpq $0, %rax
    je if_0_else
    movq -16(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    jmp .end_ack
    jmp if_0_end
if_0_else:
if_0_end:
    movq -16(%rbp), %rax
    pushq %rax
    movq $0, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    sete %al
    movzbq %al, %rax
    cmpq $0, %rax
    je if_1_else
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    pushq %rax
    movq $1, %rax
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call ack
    jmp .end_ack
    jmp if_1_end
if_1_else:
if_1_end:
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    pushq %rax
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call ack
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call ack
    jmp .end_ack
.end_ack:
    leave
    ret
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $0, -8(%rbp)
    movq $0, -16(%rbp)
    movq $0, %rax
    movq %rax, -8(%rbp)
while_0_start:
    movq -8(%rbp), %rax
    pushq %rax
    movq $3, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setle %al
    movzbq %al, %rax
    cmpq $0, %rax
    je while_0_end
    movq $0, %rax
    movq %rax, -16(%rbp)
while_1_start:
    movq -16(%rbp), %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setle %al
    movzbq %al, %rax
    cmpq $0, %rax
    je while_1_end
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
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call ack
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -16(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -16(%rbp)
    jmp while_1_start
while_1_end:
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
