/*
 * CPU.CPP
 * DESC: This file contains the standard main entry point for our CPU.
 * Primary Author: Ray Healy
 * Contributor: Ian Campbell
 */
#include "includes.h"
#include "cpu.h"

#define kDefMicroCode "uCode.obj"
#define kDefUTrace false

int main(int argc, char** argv) {

//    CPUObject::debug |=  CPUObject::memload;
    cout << hex;

    if(argc <= 1 || argc > 4) {
        cout << "Usage: " << argv[0] << " <code file> [microTrace on (y/n)] " 
             << "[microcode file]" << endl;
        return EXIT_FAILURE;
    }
    
    connect();
    
    if(argc == 2) {
        execute(argv[1], kDefMicroCode, kDefUTrace);
    } else if(argc == 3) {
        execute(argv[1], kDefMicroCode, argv[2][0] == 'y');
    } else {
        execute(argv[1], argv[3], argv[2][0] == 'y');
    }
    
    return EXIT_SUCCESS;
}
