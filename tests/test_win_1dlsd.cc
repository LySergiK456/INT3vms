#include <memory>
#include "../include/1d-lsd/1dlsd.hh"
#include "../include/1d-lsd/windows/assid.hh"
#include "../include/pe64/pe64.hh"
extern "C" {
	#include "../include/1d-lsd/binary.h"
};

namespace SYX = SlitYaX86;
using WLSD = SYX::w1Dlsd;

struct WObfuscA
	: SYX::acid_pass
{
    WObfuscA() = default;
    LSD_STATUS _init_(WLSD *lsd) {
    	puts("+");
    	return LSD_OK;
    }
};

int main() {
	// _pe64_image
	auto pe = std::make_shared<PE64>("./specimen/b.exe", PE_SYMTAB_DIR);
	auto pass = WObfuscA();
	// make arg=pass a reference
	auto lsd = SYX::w1Dlsd(&pass, pe);
	// lsd.run();
	// lsd.make("/tmp/hello.out");
	return 0;
}