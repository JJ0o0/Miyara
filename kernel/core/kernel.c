volatile char* video = (volatile char*)0xB8000;

void kernel_main(void) {
    video[0] = 'M';

    while (1) {}
}