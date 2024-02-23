#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_calculator = false;

void calculator_play() {
    is_writing_command = false;
    while (in_calculator) {
        sleep(100); // sleep for 1s
    }
}  

void calculator_calculate() {
    int len = strlen(command);

    int num1 = 0;
    int num2 = 0;

    int calc_type = 0;

    char buffer[256];

    for (int i = 0; i < len; i++) {
        if (command[i] == '+') {
            calc_type = 1;
            num1 = atoi(buffer);
            break;
        } else if (command[i] == '-') {
            calc_type = 2;
            num1 = atoi(buffer);
            break;
        } else if (command[i] == '*') {
            calc_type = 3;
            num1 = atoi(buffer);
            break;
        } else if (command[i] == '/') {
            calc_type = 4;
            num1 = atoi(buffer);
            break;
        }

        buffer[i] = command[i];
    }

    int len2 = strlen(buffer);

    for (int i = 0; i < len; i++) {
        buffer[i] = command[i + len2 + 1];
    }

    num2 = atoi(buffer);

    int result = 0;

    if (calc_type == 1) {
        result = num1 + num2;
    } else if (calc_type == 2) {
        result = num1 - num2;
    } else if (calc_type == 3) {
        result = num1 * num2;
    } else if (calc_type == 4) {
        result = num1 / num2;
    }

    char result_str[256];
    itoa(result, result_str, 10);

    terminal_writestring("Result: ");
    terminal_writestring(result_str);

    in_calculator = false;
}

void calculator_start() {
    in_calculator = true;
}



