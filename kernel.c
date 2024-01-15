int main() {
    int i = 0;
    char* baseAddress = (char*) 0xb8000;
    for (i; i < 2000; i++)
    {
        *(baseAddress + i * 2)  = ' ';
        *(baseAddress + i * 2 + 1)  = 0xdf;
    }
    
    return 0; 
}

int test() {
    char string[] = "Hello";
    int i = 0;
    char* baseAddress = (char*) 0xb8600;
    int j = 0;
    while (string[j] != '\0') {
        *(baseAddress + (i + j) * 2) = string[j];
        *(baseAddress + (i + j) * 2 + 1) = 0xdf;
        j++;
    }
    i += j;
    return 0;
}