// header file- contains only declarations

#include <cstdint> // uints are to be preferred for bitmasks.
#include <fstream> // file handling
#include <iostream> // for cout mainly
#include <vector> 
#include <random> // rng
#include <chrono>   
#include <SDL2/SDL.h>

using std::cout;

const unsigned int MEMORY_SIZE = 4096;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
const uint16_t ROM_START_ADDRESS = 0x200;
const uint16_t FONT_START_ADDRESS = 0x050; // 0x050-0x0A0
const uint8_t fontset[16][5] = {{ // built-in character fonts 

            0b11110000,
            0b10010000,
            0b10010000,
            0b10010000,
            0b11110000
        },   
        {
            0b00100000,
            0b01100000,
            0b00100000,
            0b00100000,
            0b01110000
        },
        {
            0b11110000,
            0b00010000,
            0b11110000,
            0b10000000,
            0b11110000
        },
        {
            0b11110000,
            0b00010000,
            0b11110000,
            0b00010000,
            0b11110000
        },
        {
            0b10010000,
            0b10010000,
            0b11110000,
            0b00010000,
            0b00010000
        },
        {
            0b11110000,
            0b10000000,
            0b11110000,
            0b00010000,
            0b11110000
        },
        {
            0b11110000,
            0b10000000,
            0b11110000,
            0b10010000,
            0b11110000    
        },
        {
            0b11110000,
            0b00010000,
            0b00100000,
            0b01000000,
            0b01000000
        },
        {
            0b11110000,
            0b10010000,
            0b11110000,
            0b10010000,
            0b11110000
        },
        {
            0b11110000,
            0b10010000,
            0b11110000,
            0b00010000,
            0b11110000
        },
        {
            0b11110000,
            0b10010000,
            0b11110000,
            0b10010000,
            0b10010000
        },
        {
            0b11100000,
            0b10010000,
            0b11100000,
            0b10010000,
            0b11100000
        },
        {
            0b11110000,
            0b10000000,
            0b10000000,
            0b10000000,
            0b11110000
        },
        {
            0b11100000,
            0b10010000,
            0b10010000,
            0b10010000,
            0b11100000
        },
        {
            0b11110000,
            0b10000000,
            0b11110000,
            0b10000000,
            0b11110000
        },
        {
            0b11110000,
            0b10000000,
            0b11110000,
            0b10000000,
            0b10000000,
        }
    };

// debugging variables
int ddraw = 0;
int dcycles = 0;

// RNG
std::random_device rand_device; // to obtain seed for non-deterministic RNG (as opposed to pseudo RNG)
std::mt19937 rand_int_generator(rand_device()); // Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<> rand_distrib(0, 255);
// RNG usage: uint8_t random_number = rand_distrib(rand_int_generator);

// probably making this a class will help keep things together and connected later on 
class Chip8 {
    public:
    Chip8();
    void loadROM(char*);
    void printMemory();
    // returns video
    uint32_t* ret_video();
    // returns keypad
    uint8_t* ret_keypad();

    // Opcodes
    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();

    // Pointer Table Functions
    void FT_0();
    void FT_8();
    void FT_E();
    void FT_F();

    // Null Function
    void OP_NULL();

    // Denotes 1 cycle
    void chip8_cycle();

    private:
    uint8_t registers[16] {}; // main registers 0-F
    uint16_t index_register {}; 
    uint8_t memory[MEMORY_SIZE] {}; // as size remains constant static arrays are good enough
    uint32_t video[VIDEO_HEIGHT * VIDEO_WIDTH] {}; // 4 bytes per pixel is used to make it easier to work with SDL. Each pixel is either ON or OFF
    uint16_t pc {};    
    uint16_t opcode {}; // 2 byte opcode to be executed next
    uint8_t sp {};
    uint16_t pc_stack[16] {};
    uint8_t keypad[16] {};
    uint8_t delay_timer {};
    uint8_t sound_timer {};
    // function pointer table
    typedef void (Chip8::*Chip8_Function)();   
    Chip8_Function ftable[16] {};
    Chip8_Function ftable_0[0xF] {};
    Chip8_Function ftable_8[0xF] {};
    Chip8_Function ftable_E[0xF] {};
    Chip8_Function ftable_F[0x66] {};
};

class SDLx {
    private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    public:
    SDLx(const char* title, int window_width, int window_height);
    ~SDLx();
    void cycle_end_update(void const* chip8_video, int pitch);
    bool keyboard_input(uint8_t* chip8_keypad);
};