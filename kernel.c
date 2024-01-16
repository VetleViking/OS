int test(char string[]);
int test2();
int i = 0;

int main() {
    char string[] = "Hello";
    char* baseAddress = (char*) 0xb8000;
    for (i; i < 1995; i++)
    {
        *(baseAddress + i * 2)  = ' ';
        *(baseAddress + i * 2 + 1)  = 0xdf;
    }

    test(string);
    test2();
    
    return 0; 
}

int test(char string[]) {
    char* baseAddress = (char*) 0xb8000;
    int j = 0;
    while (string[j] != '\0') {
        *(baseAddress + (i + j) * 2) = string[j];
        *(baseAddress + (i + j) * 2 + 1) = 0xdf;
        j++;
    }
    i += j;
    return 0;
}

int test2() {
    char* baseAddress = (char*) 0xb8000;
    int j = 0;
    while (j < 5) {
        *(baseAddress + (i + j) * 2) = 'a';
        *(baseAddress + (i + j) * 2 + 1) = 0xdf;
        j++;
    }
    i += j;
    return 0;
}