#include "Program.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbimage.h"

int main() {
    Program program;
    program.init(Size(1920,1080),"SomeA",true);
    program.start();
    return 0;
}


