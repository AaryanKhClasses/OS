extern "C" void kernel_main() {
    const char* msg = "Hello World!";
    char* video_memory = (char*)0xb8000; // VGA text mode memory address

    // Clear Screen
    for(int i = 0; i < 80 * 25; i++) {
        video_memory[i * 2] = ' '; // Clear character
        video_memory[i * 2 + 1] = 0x07; // Light gray on black background
    }

    for(int i = 0; msg[i] != '\0'; i++) {
        video_memory[i * 2] = msg[i]; // Character
        video_memory[i * 2 + 1] = 0x07; // Light gray on black background
    }

    while(true) {}
}
