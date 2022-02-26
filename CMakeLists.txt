cmake_minimum_required(VERSION 3.21)
project(CS_2200 C)

set(CMAKE_C_STANDARD 99)

add_executable(CS_2200 main.c
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/main.c"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/main.h"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist.c"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist.h"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist_tests.c"
        "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/arraylist_tests.h" "Homeworks/CS 2200 HW 4 - C Practice/C_Review_Homework/src/testing_header.h")
