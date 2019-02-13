;;; .data
.word 0                         ; ISR address 0

;;; stack for 2 processes
.word 0                         ; 1
.word 0                         ; 2
.word 0                         ; 3
.word 0                         ; 4
.word 0                         ; 5
.word 0                         ; 6
A_stack_top:

.word 0                         ; 7
.word 0                         ; 8
.word 0                         ; 9
.word 0                         ; 10
.word 0                         ; 11
.word 0                         ; 12
B_stack_top:

current:
.word 0                         ; current proc num 13 
process_control_block:
.word 0                         ; saved sp(A) 14
.word 0                         ; saved sp(B) 15

;;; .text
OS_start:
    ;; setup ISR
    movei   R0, 0                 ; 16
    la      R1, save_context      ; 17
    sw      R0, R1, 0             ; 18

    ;; setup initial context for Process B
    la      sp, B_stack_top     ; sp <- B_stack_top 19
    ;; save PSR and PC
    movei   R1, 1               ; bit 1: Interrupt Enabled 20
    push    R1                  ; 21
    la      R1, proc_start      ; initial PC points to process entry point 22
    push    R1                  ; 23

    ;; save(clear) general purpose registers
    movei   R1, 0               ; 24
    push    R1			; Push for R0   25
    push    R1			; Push for R1   26
    push    R1			; Push for R2   27
    movei   R3, 66              ; R3 <- 'B' 28
    push    R3                  ; save R3 so that after restoring the context, proc A should see 'A' and proc B should see 'B' 29

    ;; save sp 
    la      R2, process_control_block ; 30
    addi    R2, R2, 1		; sp is saved to process_control_block[1] for context switch 31
    sw      R2, sp, 0                 ; 32

    ;; Execute Process A first
    ;;   First, set up the current process id (0) for Process A 
    movei   R0, 0		                  ; 33
    la      R1, current               ; 34
    sw      R1, R0, 0                 ; 35 
    ;;   Second, set up the inital value of R3 (for print 'A') 
    movei  R3, 65		; R3 <- 'A'       ; 36
    ;;   Third, set up sp of Process A
    la     sp, A_stack_top            ; 37
    ;;   Finally, use iret to return back to user
    movei   R1, 1               ; bit 1: Interrupt Enabled 38
    push    R1                  ; 39
    la      R1, proc_start      ; initial PC points to process entry point 40
    push    R1                  ; 41
    iret                        ; 42

save_context:
    ;; save general purpose registers
    push    R0                  ; 43
    push    R1                  ; 44 
    push    R2                  ; 45
    push    R3                  ; 46

    la      R0, current         ; 47
    lw      R0, R3, 0           ; R3 <- current process id 48

    ;; save sp
    la      R0, process_control_block ; 49
    add     R3, R0              ; R0 <- addr of process_control_block[current] 50
    sw      R0, sp, 0           ; process_control_block[current] <- sp 51
    ;; set R2 as the next process id
    movei    R2, 1              ; if R3 == 0 then R2 <- 1 else 0 52
    blez    R3, switch_proc     ; 53
    movei    R2, 0              ; 54
switch_proc:                 ; R2 = next process id
    ;; update current process id
    la      R0, current         ; 55
    sw      R0, R2, 0           ; current <- R2 56
    ;; restore sp
    la      R0, process_control_block  ; 57
    add     R2, R0              ; R0 <- addr of process_control_block[next]
    lw      R0, sp, 0           ; sp <- process_control_block[next]
    ;; restore general purpose registers
    pop     R3
    pop     R2
    pop     R1
    pop     R0
    ;; go back to user
    iret

proc_start:                     ; entry point of user process
    put     R3                  ; print the char in R3 on the screen  65
    jmp     proc_start          ; 66
