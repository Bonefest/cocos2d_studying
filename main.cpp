#include "Program.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbimage.h"

int main() {
    Program program;
    program.init(Size(640,480),"SomeA",false);
    program.start();
    return 0;
}


