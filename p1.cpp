// Implementation

#include "p1.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <ROMfile>\n", argv[0]);
        return 0;
    }
    int scale_factor = 10;
    int cycle_delay = 1;

    SDLx sdlx("Chip-8 Emulator", VIDEO_WIDTH * scale_factor, VIDEO_HEIGHT * scale_factor);
    Chip8 chip8;
    chip8.loadROM(argv[1]);
    // chip8.printMemory();

    int video_pitch = (sizeof(uint32_t) * VIDEO_WIDTH);

    bool quit = false;

    auto last_cycle_time = std::chrono::high_resolution_clock::now();
    long long tmp = 1;
    while (!quit) {
        quit = sdlx.keyboard_input(chip8.ret_keypad());
        if (quit) {
            cout << "bye!\n";
        }

        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();
        
        if (dt > cycle_delay) {
            // cout << "cycle " << tmp << "\n";
            tmp++;
            last_cycle_time = current_time;
            chip8.chip8_cycle();
            sdlx.cycle_end_update(chip8.ret_video(), video_pitch);
        }
    }

    return 0;
}   

SDLx::SDLx(const char* title, int window_width, int window_height) {
    SDL_Init(SDL_INIT_VIDEO);
    // window properties
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
    // rendering context or properties apart from pixel color info for rendering in the specified window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // texture is just the pixel color info for rendering
    // Pixel format: (MSB) Red-Green-Blue-Alpha (LSB) 1 byte for each
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);    

}

SDLx::~SDLx() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLx::cycle_end_update(void const* chip8_video, int pitch) {
    // Update texture (pixel info)
    SDL_UpdateTexture(texture, nullptr, chip8_video, pitch);
    // Clear renderer
    SDL_RenderClear(renderer);
    // Copy texture to renderer
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    // Draw renderer on screen
    SDL_RenderPresent(renderer);
}

bool SDLx::keyboard_input(uint8_t* chip8_keypad) {
    SDL_Event *event = new SDL_Event;
    bool ans = false;
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            ans = true;
        }
        else if (event->type == SDL_KEYDOWN) {
            switch((event->key).keysym.sym) {
                case SDLK_ESCAPE: 
                    ans = true;
                    break;
                case SDLK_1:
                    chip8_keypad[1] = 1;
                    break;
                case SDLK_2:
                    chip8_keypad[2] = 1;
                    break;                
                case SDLK_3:
                    chip8_keypad[3] = 1;
                    break;                
                case SDLK_4:
                    chip8_keypad[0xC] = 1;
                    break;                
                case SDLK_q:
                    chip8_keypad[4] = 1;
                    break;                
                case SDLK_w:
                    chip8_keypad[5] = 1;
                    break;                
                case SDLK_e:
                    chip8_keypad[6] = 1;
                    break;                
                case SDLK_r:
                    chip8_keypad[0xD] = 1;
                    break;                
                case SDLK_a:
                    chip8_keypad[7] = 1;
                    break;                
                case SDLK_s:
                    chip8_keypad[8] = 1;
                    break;                
                case SDLK_d:
                    chip8_keypad[9] = 1;
                    break;                
                case SDLK_f:
                    chip8_keypad[0xE] = 1;
                    break;                
                case SDLK_z:
                    chip8_keypad[0xA] = 1;
                    break;                
                case SDLK_x:
                    chip8_keypad[0] = 1;
                    break;                
                case SDLK_c:
                    chip8_keypad[0xB] = 1;
                    break;                
                case SDLK_v:
                    chip8_keypad[0xF] = 1;
                    break;                
            }
        }
        else if (event->type == SDL_KEYUP) {
            switch ((event->key).keysym.sym) {
                case SDLK_1:
                    chip8_keypad[1] = 0;
                    break;
                case SDLK_2:
                    chip8_keypad[2] = 0;
                    break;                
                case SDLK_3:
                    chip8_keypad[3] = 0;
                    break;                
                case SDLK_4:
                    chip8_keypad[0xC] = 0;
                    break;                
                case SDLK_q:
                    chip8_keypad[4] = 0;
                    break;                
                case SDLK_w:
                    chip8_keypad[5] = 0;
                    break;                
                case SDLK_e:
                    chip8_keypad[6] = 0;
                    break;                
                case SDLK_r:
                    chip8_keypad[0xD] = 0;
                    break;                
                case SDLK_a:
                    chip8_keypad[7] = 0;
                    break;                
                case SDLK_s:
                    chip8_keypad[8] = 0;
                    break;                
                case SDLK_d:
                    chip8_keypad[9] = 0;
                    break;                
                case SDLK_f:
                    chip8_keypad[0xE] = 0;
                    break;                
                case SDLK_z:
                    chip8_keypad[0xA] = 0;
                    break;                
                case SDLK_x:
                    chip8_keypad[0] = 0;
                    break;                
                case SDLK_c:
                    chip8_keypad[0xB] = 0;
                    break;                
                case SDLK_v:
                    chip8_keypad[0xF] = 0;
                    break;        
            }
        }
    }   
    delete event;
    return ans; 
}

Chip8::Chip8() {
    // pre-setting pc to the start of ROM contents
    pc = ROM_START_ADDRESS;

    // pre-setting stack pointer to 0
    sp = 0;

    // pre-loading 16 fonts for built in characters: 5 bytes each
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 5; j++) {
            memory[FONT_START_ADDRESS + (5 * i) + j] = fontset[i][j];
        }
    }

    // Initializing function pointer tables
    ftable[0] = &Chip8::FT_0;
    ftable[8] = &Chip8::FT_8;
    ftable[0xE] = &Chip8::FT_E;
    ftable[0xF] = &Chip8::FT_F;

    ftable[1] = &Chip8::OP_1nnn;
    ftable[2] = &Chip8::OP_2nnn;
    ftable[3] = &Chip8::OP_3xkk;
    ftable[4] = &Chip8::OP_4xkk;
    ftable[5] = &Chip8::OP_5xy0;
    ftable[6] = &Chip8::OP_6xkk;
    ftable[7] = &Chip8::OP_7xkk;
    ftable[9] = &Chip8::OP_9xy0;
    ftable[0xA] = &Chip8::OP_Annn;
    ftable[0xB] = &Chip8::OP_Bnnn;
    ftable[0xC] = &Chip8::OP_Cxkk;
    ftable[0xD] = &Chip8::OP_Dxyn;

    for (unsigned int i = 0; i < 0xFu; i++)
    {
        ftable_0[i] = &Chip8::OP_NULL;
        ftable_8[i] = &Chip8::OP_NULL;
        ftable_E[i] = &Chip8::OP_NULL;
    }
    for (int i = 0; i < 0x66; i++)
    {
        ftable_F[i] = &Chip8::OP_NULL;
    }

    ftable_0[0u] = &Chip8::OP_00E0;
    ftable_0[0xEu] = &Chip8::OP_00EE;

    ftable_8[0u] = &Chip8::OP_8xy0;
    ftable_8[1u] = &Chip8::OP_8xy1;
    ftable_8[2u] = &Chip8::OP_8xy2;
    ftable_8[3u] = &Chip8::OP_8xy3;
    ftable_8[4u] = &Chip8::OP_8xy4;
    ftable_8[5u] = &Chip8::OP_8xy5;
    ftable_8[6u] = &Chip8::OP_8xy6;
    ftable_8[7u] = &Chip8::OP_8xy7;
    ftable_8[0xEu] = &Chip8::OP_8xyE;

    ftable_E[1u] = &Chip8::OP_ExA1;
    ftable_E[0xEu] = &Chip8::OP_Ex9E;

    ftable_F[0x07u] = &Chip8::OP_Fx07;
    ftable_F[0x0Au] = &Chip8::OP_Fx0A;
    ftable_F[0x15u] = &Chip8::OP_Fx15;
    ftable_F[0x18u] = &Chip8::OP_Fx18;
    ftable_F[0x1Eu] = &Chip8::OP_Fx1E;
    ftable_F[0x29u] = &Chip8::OP_Fx29;
    ftable_F[0x33u] = &Chip8::OP_Fx33;
    ftable_F[0x55u] = &Chip8::OP_Fx55;
    ftable_F[0x65u] = &Chip8::OP_Fx65;
}

void Chip8::loadROM(char* filename) {
    std::ifstream file(filename, std::ios::binary|std::ios::ate);
    if (!file.is_open()) {
        cout << "ERROR: Could not open ROM file\n";
    }
    // tested some files and file size doesn't seem to go past ~5000. So ll is more than safe enough
    long long size = file.tellg();
    file.seekg(0, std::ios::beg);
    char* buffer = new char[size];
    file.read(buffer, size);
    file.close();
    // // printing each byte for fun!
    // for (int i = 0; i < size; i++)
    // {
    //     char tmp = buffer[i];
    //     printf("%u\n", (unsigned char) buffer[i]);
    //     printf("%u\n", (int) buffer[i]);
    //     for (int j = 7; j >= 0; j--)
    //     {
    //         cout << ((tmp >> j) & 1u);
    //     }
    //     cout << "\n";
    // }
    for (int i = 0; i < size; i++)
    {
        // ROM content is stored starting at 0x200
        memory[ROM_START_ADDRESS + i] = buffer[i];
    }
    delete[] buffer;
}

void Chip8::chip8_cycle() {
    // Fetching next opcode to be executed from memory
    uint16_t instruction;
    instruction = memory[pc];
    instruction <<= 8u;
    instruction &= memory[pc + 1];

    // Incrementing program counter
    pc += 2;

    // Executing opcode
    ((*this).*(ftable[(instruction & 0xF000u) >> 12u]))();

    // Updating delay timer
    if (delay_timer > 0) {
        delay_timer--;
    }

    // Updating sound timer
    if (sound_timer > 0) {
        sound_timer--;
    }
}

void Chip8::printMemory() { // This function is only used for debugging purposes
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        cout << ((int) memory[i]) << " ";
        if ((i + 1) % 32 == 0) {
            cout << "\n";
        }
    }
    cout << "\n";
}

// Return video object of Chip8 class
uint32_t* Chip8::ret_video() {
    return video;
}

// Returns keypad object of Chip8 Class
uint8_t* Chip8::ret_keypad() {
    return keypad;
}

// Implementing 34 opcodes

void Chip8::OP_00E0() {
    // CLS    
    for (int i = 0; i < VIDEO_HEIGHT; i++)
    {
        for (int j = 0; j < VIDEO_WIDTH; j++)
        {
            video[i * VIDEO_WIDTH + j] = 0;
        }
    }
}

void Chip8::OP_00EE() {
    // RET
    sp--;
    pc = pc_stack[sp];
}

void Chip8::OP_1nnn() {
    // JMP
    uint16_t address = (opcode & 0x0FFFu);
    pc = address;
}

void Chip8::OP_2nnn() {
    // CALL
    uint16_t address = (opcode & 0x0FFFu);
    pc_stack[sp] = pc;
    sp++;
    pc = address;
}

void Chip8::OP_3xkk() {
    // Skip next instruction if Register Vx == kk
    uint8_t x = ((opcode & 0x0F00u) >> 8u);
    uint8_t kk = (opcode & 0x00FFu);
    if (registers[x] == kk) {
        pc += 2;    
    }
}

void Chip8::OP_4xkk() {
    // Skip next instruction if Register Vx != kk
    uint8_t x = ((opcode & 0x0F00u) >> 8u);
    uint8_t kk = (opcode & 0x00FFu);
    if (registers[x] != kk) {
        pc += 2;    
    }
}

void Chip8::OP_5xy0() {
    // Skip next if Vx == Vy
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    if (registers[x] == registers[y]) {
        pc += 2;
    }
}

void Chip8::OP_6xkk() {
    // Set Vx = kk
    uint8_t x = ((opcode & 0x0F00u) >> 8u);
    uint8_t kk = (opcode & 0x00FFu);
    registers[x] = kk;
}

void Chip8::OP_7xkk() {
    // Set Vx = Vx + kk
    uint8_t x = ((opcode & 0x0F00u) >> 8u);
    uint8_t kk = (opcode & 0x00FFu);
    registers[x] += kk;
}

void Chip8::OP_8xy0() {
    // Set Vx = Vy
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    registers[x] = registers[y];
}

void Chip8::OP_8xy1() {
    // Set Vx = Vx | Vy
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    registers[x] |= registers[y];
}

void Chip8::OP_8xy2() {
    // Set Vx &= Vy
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    registers[x] &= registers[y];
}

void Chip8::OP_8xy3() {
    // Set Vx ^= Vy
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    registers[x] ^= registers[y];
}

void Chip8::OP_8xy4() {
    // Set Vx += Vy; if result is greater than 8 bits then set VF = 1, else 0
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    uint16_t sum = 0x0000u + x + y;
    if (sum > 0x00FFu) {
        registers[0xFu] = 1u;
    }
    else {
        registers[0xFu] = 0u;
    }
    registers[x] = (sum & 0x00FFu);
}

void Chip8::OP_8xy5() {
    // Set Vx -= Vy; if Vx > Vy initially then set VF = 1, else 0;
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    if (registers[x] > registers[y]) {
        registers[0xFu] = 1u;
    }
    else {
        registers[0xFu] = 0u;
    }
    registers[x] -= registers[y];
}

void Chip8::OP_8xy6() {
    // Set Vx >>= 1; If initially LSB of Vx is 1, then set VF = 1, else 0
    uint8_t x;
    x = ((opcode && 0x0F00u) >> 8u);
    if ((registers[x] & 0b1u) == 1) {
        registers[0xFu] = 1;
    }
    else {
        registers[0xFu] = 0;
    }
    registers[x] >>= 1;
}

void Chip8::OP_8xy7() {
    // If Vy > Vx, set VF = 1 else 0; Vx = Vy - Vx
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    if (registers[y] > registers[x]) {
        registers[0xFu] = 1;
    }
    else {
        registers[0xFu] = 0;
    }
    registers[x] = registers[y] - registers[x]; 
}

void Chip8::OP_8xyE() {
    // Set Vx <<= 1; If initially MSB of Vx is 1, then set VF = 1, else 0
    uint8_t x;
    x = ((opcode && 0x0F00u) >> 8u);
    if ((registers[x] & (1u << 7)) != 0u) {
        registers[0xFu] = 1;
    }
    else {
        registers[0xFu] = 0;
    }
    registers[x] <<= 1;
}

void Chip8::OP_9xy0() {
    // Skip next if Vx != Vy
    uint8_t x, y;
    x = ((opcode && 0x0F00u) >> 8u);
    y = ((opcode && 0x00F0u) >> 4u);
    if (registers[x] != registers[y]) {
        pc += 2;
    }
}

void Chip8::OP_Annn() {
    // Set Index register = nnn
    uint16_t address = (opcode & 0x0FFFu);
    index_register = address;
}

void Chip8::OP_Bnnn() {
    // Jump to location nnn + V0
    uint16_t address = (opcode & 0x0FFFu);
    pc = registers[0u] + address;
}

void Chip8::OP_Cxkk() {
    // Vx = Random Byte & kk
    uint8_t x = ((opcode & 0x0F00u) >> 8u);
    uint8_t kk = (opcode & 0x00FFu);
    uint8_t rand_byte = rand_distrib(rand_int_generator);
    registers[x] = (rand_byte & kk);
}

void Chip8::OP_Dxyn() {
    // Draw sprite of height n starting at location (Vx, Vy) consisting of n bytes. If collision set VF = 1, else 0. Sprite can be copied starting at location VI
    uint8_t x, y, n;
    x = (opcode & 0x0F00u) >> 8u;
    y = (opcode & 0x00F0u) >> 4u;
    n = opcode & 0x000Fu;
    // It's a bit confusing, think of x and y as coordinates (same for xx, xxx, yy, yyy) 
    uint8_t xx = registers[x] % VIDEO_WIDTH; // starting location of drawing in coordinates
    uint8_t yy = registers[y] % VIDEO_HEIGHT;
    // A Chip8 sprite is characteristically always 8 pixels wide, with varying height
    registers[0xFu] = 0;
    for (uint8_t i = 0; i < n; i++)
    {
        uint8_t sprite_byte = memory[index_register + i];
        for (uint8_t j = 0; j < 8; j++)
        {
            uint8_t xxx = (xx + j) % VIDEO_WIDTH; // current location of drawing in coordinates
            uint8_t yyy = (yy + i) % VIDEO_HEIGHT; 
            uint8_t sprite_bit = (sprite_byte & (1u << (7 - j))) >> (7 - j);
            if (sprite_bit == 1u) {
                if (video[yyy * VIDEO_WIDTH + xxx] != 0u) {
                    // collision
                    registers[0xFu] = 1u; 
                }       
                video[xxx * VIDEO_WIDTH + yyy] ^= 0xFFFFFFFFu;
            }
        }
    }
}
void Chip8::OP_Ex9E() {
    // skip next if key on keypad with value of Vx is pressed
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[x];
    if (keypad[key]) {
        pc += 2u;
    }
}

void Chip8::OP_ExA1() {
    // skip next if key on keypad with value of Vx is not pressed
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[x];
    if (!keypad[key]) {
        pc += 2u;
    }    
}

void Chip8::OP_Fx07() {
    // Set Vx = delay timer value
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    registers[x] = delay_timer;
}

void Chip8::OP_Fx0A() {
    // Wait for a keypress, store the value of key in Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for (unsigned int i = 0u; i < 16u; i++)
    {
        if (keypad[i]) {
            registers[x] = i;
            return;
        }
    }
    pc -= 2;
}

void Chip8::OP_Fx15() {
    // Set delay timer = Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    delay_timer = registers[x];
}

void Chip8::OP_Fx18() {
    // Set sound timer = Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    sound_timer = registers[x];
}

void Chip8::OP_Fx1E() {
    // Set I += Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    index_register += registers[x];
}

void Chip8::OP_Fx29() {
    // Set I = location of sprite for digit Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[x];
    index_register = FONT_START_ADDRESS + (digit * 5u);
}

void Chip8::OP_Fx33() {
    // stores hundredth digit in location I, tenth in I+1, and ones in I+2 of memory for the number in Vx 
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t num = registers[x];
    uint8_t dig = num % 10;
    memory[index_register + 2] = dig;
    num /= 10u;
    dig = num % 10u;
    memory[index_register + 1] = dig;
    num /= 10u;
    dig = num % 10u;
    memory[index_register] = dig;
}

void Chip8::OP_Fx55() {
    // Copy registers V0 to Vx inclusive starting at location VI
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    for (unsigned int i = 0; i <= x; i++)
    {
        memory[index_register + i] = registers[x];
    }
}

void Chip8::OP_Fx65() {
    // Copy into registers V0 to Vx from memory starting at location VI
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    for (unsigned int i = 0; i <= x; i++)
    {
        registers[i] = memory[index_register + i];  
    }
}

// Implementing Function Pointer Functions
void Chip8::FT_0() {
    uint8_t last_digit = (opcode & 0x000Fu); 
    ((*this).*(ftable_0[last_digit]))();
}

void Chip8::FT_8() {    

    uint8_t last_digit = (opcode & 0x000Fu); 
    ((*this).*(ftable_8[last_digit]))();
}

void Chip8::FT_E() {

    uint8_t last_digit = (opcode & 0x000Fu); 
    ((*this).*(ftable_E[last_digit]))();
}

void Chip8::FT_F() {
    uint8_t last_digit = (opcode & 0x00FFu);
    ((*this).*(ftable_F[last_digit]))();
}

// Null Function
void Chip8::OP_NULL() {
    // if non-existing function called then this will get called
}