cmake_minimum_required(VERSION 3.21)
project(CS_2200 C)

set(CMAKE_C_STANDARD 99)

add_executable(CS_2200 main.c
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/main.c"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/main.h"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist.c"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist.h"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist_tests.c"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist_tests.h"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/testing_header.h"
        "Projects/Project 3 - Virtual Memory/project3headertomakeeverythingwork.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/mmu.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/pagesim.c"
        "Projects/Project 3 - Virtual Memory/simulator-src/pagesim.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/proc.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/swap.c"
        "Projects/Project 3 - Virtual Memory/simulator-src/swap.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/stats.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/swapops.c"
        "Projects/Project 3 - Virtual Memory/simulator-src/swapops.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/types.h"
        "Projects/Project 3 - Virtual Memory/simulator-src/util.c"
        "Projects/Project 3 - Virtual Memory/simulator-src/util.h"
        "Projects/Project 3 - Virtual Memory/student-src/va_splitting.h"
        "Projects/Project 3 - Virtual Memory/student-src/mmu.c"
        "Projects/Project 3 - Virtual Memory/student-src/page_replacement.c"
        "Projects/Project 3 - Virtual Memory/student-src/proc.c"
        "Projects/Project 3 - Virtual Memory/student-src/page_fault.c"
        "Projects/Project 3 - Virtual Memory/student-src/stats.c")
