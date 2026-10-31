#include <memory>
#include "../include/1d-lsd/1dlsd.hh"
#include "../include/1d-lsd/windows/assid.hh"
#include "../include/pe64/pe64.hh"
extern "C" {
	#include "../include/1d-lsd/binary.h"
};

using SYX = SlitYaX86;

struct WinObfuscA
	: SYX::acid_pass
{
    WinObfuscA() = default;
    LSD_STATUS _init_(XLSD *lsd) {
    	puts("+");
    	return LSD_OK;
    }
};

int main() {
	auto pe = std::make_shared<PE64>(const_cast<char*>("./specimen/b.exe"), PE_SYMTAB_DIR);
	auto pass = WinObfuscA();
	// make arg=pass a reference
	auto lsd = SYX::x1Dlsd(&pass, pe);
	// lsd.run();
	// lsd.make("/tmp/hello.out");
	return 0;
}