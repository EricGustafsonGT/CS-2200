! Spring 2022 Revisions by Andrej Vrtanoski

! This program executes pow as a test program using the LC 22 calling convention
! Check your registers ($v0) and memory to see if it is consistent with this program

! vector table
vector0:
        .fill 0x00000000                        ! device ID 0
        .fill 0x00000001                        ! device ID 1 (should this be 1? answer: it doesn't matter, we'll use the "in" command to get the device ID anyway)
        .fill 0x00000000                        ! ...
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000
        .fill 0x00000000                        ! device ID 7
        ! end vector table

main:	lea $sp, initsp                         ! initialize the stack pointer
        lw $sp, 0($sp)                          ! finish initialization

        lea $t0, vector0                        ! DONE: Install timer interrupt handler into vector table
        lea $t1, timer_handler
        sw  $t1, 0($t0)

        lea $t1, toaster_handler                ! DONE: Install toaster interrupt handler into vector table
        sw  $t1, 1($t0)

        lea $t0, minval
        lw $t0, 0($t0)
        addi $t1, $zero, 65534                  ! store 0000ffff into minval (to make comparisons easier)
        sw $t1, 0($t0)

        ei                                      ! Enable interrupts

        lea $a0, BASE                           ! load base for pow
        lw $a0, 0($a0)
        lea $a1, EXP                            ! load power for pow
        lw $a1, 0($a1)
        lea $at, POW                            ! load address of pow
        jalr $ra, $at                           ! run pow
        lea $a0, ANS                            ! load base for pow
        sw $v0, 0($a0)

        halt                                    ! stop the program here
        addi $v0, $zero, -1                     ! load a bad value on failure to halt

BASE:   .fill 2
EXP:    .fill 8
ANS:	.fill 0                                 ! should come out to 256 (BASE^EXP)

POW:    addi $sp, $sp, -1                       ! allocate space for old frame pointer
        sw $fp, 0($sp)

        addi $fp, $sp, 0                        ! set new frame pointer

        bgt $a1, $zero, BASECHK                 ! check if $a1 is zero
        br RET1                                 ! if the exponent is 0, return 1

BASECHK:bgt $a0, $zero, WORK                    ! if the base is 0, return 0
        br RET0

WORK:   addi $a1, $a1, -1                       ! decrement the power
        lea $at, POW                            ! load the address of POW
        addi $sp, $sp, -2                       ! push 2 slots onto the stack
        sw $ra, -1($fp)                         ! save RA to stack
        sw $a0, -2($fp)                         ! save arg 0 to stack
        jalr $ra, $at                           ! recursively call POW
        add $a1, $v0, $zero                     ! store return value in arg 1
        lw $a0, -2($fp)                         ! load the base into arg 0
        lea $at, MULT                           ! load the address of MULT
        jalr $ra, $at                           ! multiply arg 0 (base) and arg 1 (running product)
        lw $ra, -1($fp)                         ! load RA from the stack
        addi $sp, $sp, 2

        br FIN                                  ! unconditional branch to FIN

RET1:   add $v0, $zero, $zero                   ! return a value of 0
	addi $v0, $v0, 1                        ! increment and return 1
        br FIN                                  ! unconditional branch to FIN

RET0:   add $v0, $zero, $zero                   ! return a value of 0

FIN:	lw $fp, 0($fp)                          ! restore old frame pointer
        addi $sp, $sp, 1                        ! pop off the stack
        jalr $zero, $ra

MULT:   add $v0, $zero, $zero                   ! return value = 0
        addi $t0, $zero, 0                      ! sentinel = 0
AGAIN:  add $v0, $v0, $a0                       ! return value += argument0
        addi $t0, $t0, 1                        ! increment sentinel
        blt $t0, $a1, AGAIN                     ! while sentinel < argument, loop again
        jalr $zero, $ra                         ! return from mult

timer_handler:                                  ! TODO 1
        !                    General Interrupt Process:
        !
        ! /* The interrupts are disabled when we enter */
        ! enable interrupts
        ! save processer registers
        ! execute device code
        ! restore processor registers
        ! disable interrupts
        ! restore $k0
        ! return from interrupt
        ! /* interrupts will be enabled by return from interrupt */

        sw      $k0, 0($sp)                     ! Save $k0
        addi    $sp, $sp, -1
        
        ei                                      ! Enable Interrupts

        sw      $t0, 0($sp)                     ! Save the state of the program NOTE: in general, all the registers should be saved here, however, for this project since we don't have a different
        addi    $sp, $sp, -1                    ! stack for the system and user, we can just save the registers that we'll use in this interrupt (as said by the TAs)
        sw      $t1, 0($sp)
        addi    $sp, $sp, -1

        ! Perform interrupt purpose. This interrupt increments the counter variable at "ticks" (0xFFFF)
        lea     $t1, ticks                      ! t1 = address that whose value is 0xFFFF (and 0xFFFF is the address of the counter)                
        lw      $t1, 0($t1)                     ! t1 = 0xFFFF 
        lw      $t0, 0($t1)                     ! t0 = value at 0xFFFF
        addi    $t0, $t0, 1
        sw      $t0, 0($t1)

        addi    $sp, $sp, 1                     ! Restore the initial state of the program before the interrupt occurred
        lw      $t1, 0($sp)
        addi    $sp, $sp, 1
        lw      $t0, 0($sp)

        di                                      ! Disable interrupts

        addi    $sp, $sp, 1                     ! Restore previous $k0 value
        lw      $k0, 0($sp)

        RETI                                    ! RETurn from Interrupt



toaster_handler:                                ! TODO 2
        ! retrieve the data from the device and check if it is a minimum or maximum value
        ! then calculate the difference between minimum and maximum value
        ! (hint: think about what ALU operations you could use to implement subract using 2s compliment)

        !                    General Interrupt Process:
        !
        ! /* The interrupts are disabled when we enter */
        ! enable interrupts
        ! save processer registers
        ! execute device code
        ! restore processor registers
        ! disable interrupts
        ! restore $k0
        ! return from interrupt
        ! /* interrupts will be enabled by return from interrupt */

        sw      $k0, 0($sp)                     ! Save $k0
        addi    $sp, $sp, -1
        
        ei                                      ! Enable Interrupts

        sw      $t0, 0($sp)                     ! Save the state of the program NOTE: in general, all the registers should be saved here, however, for this project since we don't have a different
        addi    $sp, $sp, -1                    ! stack for the system and user, we can just save the registers that we'll use in this interrupt (as said by the TAs)
        sw      $t1, 0($sp)
        addi    $sp, $sp, -1
        sw      $t2, 0($sp)
        addi    $sp, $sp, -1
        sw      $s0, 0($sp)
        addi    $sp, $sp, -1
        sw      $s1, 0($sp)
        addi    $sp, $sp, -1
        sw      $s2, 0($sp)
        addi    $sp, $sp, -1

        ! Perform interrupt purpose. This interrupt increments the counter variable at "ticks" (0xFFFF)
        in      $s0, 1                          ! obtain the most recent temperature from the toaster using the "in" instruction. Save this temperature in $t0

        lea     $t1, maxval                     ! Obtain the offest to get to the "maxval" label
        lw      $t1, 0($t1)                     ! Obtain the address of the maximum value of the toaster, 0xFFFD, from the label "maxval"
        lw      $s1, 0($t1)                     ! load the maximum value into register s1

        lea     $t2, minval                     ! Obtain the offest to get to the "minval" label
        lw      $t2, 0($t2)                     ! Obtain the address of the minimum value of the toaster, 0xFFFC, from the label "minval"
        lw      $s2, 0($t2)                     ! load the maximum value into register s2

        bgt     $s0, $s1, newMaxFound           ! if $s0 > $s1 (i.e. if new temp > old max temp) then branch to "newMaxFound"
checkIfMin:
        blt     $s0, $s2, newMinFound           ! if $s0 < $s2 (i.e. if new temp < old min temp) then branch to "newMinFound"
        br      neitherNewMinNorMax             ! the temperature value that was read is neither lower than the previous min nor greater than the old max, branch to restoring registers

        ! end of performing toaster interrupt purpose

newMaxFound:
        sw      $s0, 0($t1)                     ! store the new maximum value in 0xFFFD (pointed to by the "maxval" label that we loaded into $t1)
        nand    $t0, $s2, $s2                   ! invert the bits of the old minimum value and place it in $t0
        addi    $t0, $t0, 1                     ! for 2's complement we add 1 to get the negated versino of $t0
        add     $t0, $s0, $t0                   ! Compute $s0 + $t0 (which is effectively: new max + (-old min))
        ! br      storeNewRange                 ! If this is the very first comparison and the max and min temps are still their initial values, then this new temp is both the min and max (so we must set the min as well)
        br      neitherNewMinNorMax

newMinFound:
        sw      $s0, 0($t2)                     ! store the new minimum value in 0xFFFC (pointed to by the "minval" label that we loaded into $t2)
        lw      $s1, 0($t1)                     ! in case this is the first iteration, we must reload the max value since it was set in the block with the label "newMaxFound"
        nand    $s0, $s0, $s0                   ! invert the new minimum value (which is our new temp and is in $s0) and place it in $t0
        addi    $s0, $s0, 1                     ! for 2's complement we add 1 to get the negated version of $s0
        add     $t0, $s1, $s0                   ! Compute $t1 + $s0 (which is effectively: old max + (-new min))
                                                ! continue onto "storeNewRange"

storeNewRange:
        ! blt     $s0, $s2, newMinFound
        lea     $t1, range                      ! Obtain the offset to get to the "range" label; (we are overwriting our location to the address of the max value in MEMORY but that's ok since we don't need it anymore)
        lw      $t1, 0($t1)                     ! Obtain the address of the range of the max and min temperatures, 0xFFFE, from the label "range"
        sw      $t0, 0($t1)                     ! We don't need to load in the range value; we only want to set it. From the previous instruction $t1 contains the address to the range and from our earlier calculations $t0 contains the new range


neitherNewMinNorMax:
        addi    $sp, $sp, 1                     ! Restore the initial state of the program before the interrupt occurred
        lw      $s2, 0($sp)
        addi    $sp, $sp, 1 
        lw      $s1, 0($sp)
        addi    $sp, $sp, 1 
        lw      $s0, 0($sp)
        addi    $sp, $sp, 1
        lw      $t2, 0($sp)
        addi    $sp, $sp, 1
        lw      $t1, 0($sp)
        addi    $sp, $sp, 1
        lw      $t0, 0($sp)

        di                                      ! Disable interrupts

        addi    $sp, $sp, 1                     ! Restore previous $k0 value
        lw      $k0, 0($sp)

        RETI                                    ! RETurn from Interrupt

        


initsp: .fill 0xA000
ticks:  .fill 0xFFFF
range:  .fill 0xFFFE  ! range meaning the difference between the max and min temperatures of the toaster
maxval: .fill 0xFFFD
minval: .fill 0xFFFC