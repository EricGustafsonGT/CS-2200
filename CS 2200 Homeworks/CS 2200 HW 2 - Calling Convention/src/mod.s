!============================================================
! CS 2200 Homework 2 Part 1: mod
!
! Edit any part of this file necessary to implement the
! mod operation. Store your result in $v0.
!============================================================
!   int mod (int a, int b) {
!       int x = a;
!       while (x >= b) {
!           x = x - b;
!       }
!       return x;
!   }

mod:    addi    $a0, $zero, 28      ! $a0 = 28, the number a to compute mod(a,b)
        addi    $a1, $zero, 13      ! $a1 = 13, the number b to compute mod(a,b)

        addi    $s0, $a0, 0         ! Save parameter a (currently in $a0) in register x ($s0)
        nand    $s1, $a1, $a1       ! Invert B. note: A NAND A = NOT A
        addi    $s1, $s1, 1         ! For 2's complement we must add 1 to get the inverse number
    
WHILE:  blt     $s0, $a1, RETURN
        add     $s0, $s0, $s1       ! compute x = x + (-b)
        br      WHILE
       
RETURN: add     $v0, $s0, $zero     ! return the value by saving it in $v0
