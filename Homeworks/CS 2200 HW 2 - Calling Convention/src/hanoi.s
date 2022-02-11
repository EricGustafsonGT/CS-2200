!============================================================
! CS 2200 Homework 2 Part 2: Tower of Hanoi
!
! Apart from initializing the stack,
! please do not edit mains functionality.
!============================================================

main:
    add     $zero, $zero, $zero     ! TODO: Here, you need to get the address of the stack
                                    ! using the provided label to initialize the stack pointer.
                                    ! load the label address into $sp and in the next instruction,
                                    ! use $sp as base register to load the value (0xFFFF) into $sp.
    
    lea     $sp, stack              ! At the label "stack" is the location of the stack. Load the value of "stack" into the $sp register       add     $sp, $zero, stack     also works here
    lw      $sp, 0($sp)             ! Load the value at ADDRESS 0 + $sp into $sp (we stored the stack address into $sp previously, now we load the value = 0xFFFF into $sp)
    lea     $fp, stack
    lw      $fp, 0($fp)             ! Save the frame pointer as well
    !----------------------------------------------------------------------------
    ! everything after this until the hanoi label was given to me:
    lea     $at, hanoi              ! loads address of hanoi label into $at
    lea     $a0, testNumDisks2      ! loads address of number into $a0
    lw      $a0, 0($a0)             ! loads value of number into $a0
    jalr    $ra, $at                ! jump to hanoi, set $ra to return addr
    !lea     $ra, 1                 ! jalr is just the same as these two commented out instructions
    !BR      hanoi
    halt                            ! when we return, just halt

hanoi:
    add     $zero, $zero, $zero     ! TODO: perform post-call portion of
                                    ! the calling convention. Make sure to
                                    ! save any registers you will be using!

                                    ! t-registers should have already been saved by the caller
    sw      $a0, 0($sp)             ! Store the current activation record's parameters
    addi    $sp, $sp, -1            
    sw      $ra, 0($sp)             ! Save current return address
    addi    $sp, $sp, -1
    sw      $fp, 0($sp)             ! Save current frame pointer
    addi    $sp, $sp, -1
    ! we will not use any s reg's   ! Save any S registers we intend to use (caller is safe here)
    

    !----------------------------------------------------------------------------
    add     $zero, $zero, $zero     ! TODO: Implement the following pseudocode in assembly:
                                    ! IF ($a0 == 1)
                                    !    GOTO base
                                    ! ELSE
                                    !    GOTO else
    
    add     $t0, $zero, $zero       ! clear t0 to make it zero
    addi    $t0, $t0, 1             ! add 1 to t0

    bgt     $a0, $t0, else          ! if a0 > 1 then goto else (n will never be less than 1 so no need to check a0 < 1)
    br      base                    ! if a0 == 1 then goto base
    
else:
    add     $zero, $zero, $zero     ! TODO: perform recursion after decrementing
                                    ! the parameter by 1. Remember, $a0 holds the
                                    ! parameter value.

    addi    $a0, $a0, -1            ! compute $a0 - 1 and save back in $a0
    add     $fp, $sp, $zero         ! WE ARE ABOUT TO JUMP; save current stack pointer as the next activation record's frame pointer
    ! idc if t reg's r overwritten  ! Save t-registers
    ! no other return values        ! Save ADDITIONAL return values
    lea     $at, hanoi              ! calculate the address to jump to
    jalr    $ra, $at                ! jump
    ! None to restore               ! Restore t-registers
    ! None to restore               ! Restore Additional Parameters

    !----------------------------------------------------------------------------
    add     $zero, $zero, $zero     ! TODO: Implement the following pseudocode in assembly:
                                    ! $v0 = 2 * $v0 + 1
                                    ! RETURN $v0

    add     $v0, $v0, $v0           ! 2 * $v0 = $v0 + $v0
    addi    $v0, $v0, 1             ! increment $v0 by 1
    br      teardown                ! return

base:
    add     $zero, $zero, $zero     ! TODO: Return 1
    addi    $v0, $zero, 1           ! set $v0 to 1
    br      teardown                ! return

teardown:
    add     $zero, $zero, $zero     ! TODO: perform pre-return portion
                                    ! of the calling convention

    ! none saved; none to restore   ! Restore saved s registers
    addi    $sp, $sp, 1
    lw      $fp, 0($sp)             ! Restore previous frame pointer
    addi    $sp, $sp, 1
    lw      $ra, 0($sp)             ! Restore previous return address
    addi    $sp, $sp, 1
    lw      $a0, 0($sp)             ! Restore previous parameters
    jalr    $zero, $ra              ! return to caller



stack: .word 0xFFFF                 ! the stack begins here

! Words for testing \/

! 1
testNumDisks1:
    .word 0x0001

! 0x000A = 10
testNumDisks2:
    .word 0x000a

! 0x0014 = 20
testNumDisks3:
    .word 0x0014
