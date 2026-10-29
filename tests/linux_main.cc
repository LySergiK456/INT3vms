#include <stdio.h>
#include "../include/syx86/loader/linux/linux_ldr.hh"

namespace SYX = SlitYaX86;

int main(int argc, char **argv) {
	auto ldr = SYX::UnixLdr(argc-1, &argv[1]);
	 // auto razor = SYX::Razor<SYX::LinuxDex, SYX::UnixLdr>(argc - 1, &argv[1]);
    // auto razor = SYX::Razor(argc-1, &argv[1]);
    // printf("++%lx\n", razor->ldr.ok);
    // uc_err err = razor.Run();
    // if (!_uc_ok(err)) {
    //     puts(uc_strerror(err));
    //     return -1;
    // }
    return 0;
}