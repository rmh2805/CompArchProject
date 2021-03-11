#include "includes.h"
#include "cpu.h"

#define kDefMicroCode "uCode.obj"

int main(int argc, char** argv) {
    if(argc <= 1 || argc > 3) {
        cout << "Usage: " << argv[0] << " <code file> [microcode file]" << endl;
        return EXIT_FAILURE;
    }
    
    connect();
    
    if(argc == 2) {
        execute(argv[1], kDefMicroCode);
    } else {
        execute(argv[1], argv[2]);
    }
    
    return EXIT_SUCCESS;
}
