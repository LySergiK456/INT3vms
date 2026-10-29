WIN_BDIR = ./build/windows
LINUX_BDIR = ./build/linux

.PHONY: linux
linux:
	@make -f Makefile.linux -j$(nproc)

.PHONY: windows
windows:
	@make -f Makefile.windows -j$(nproc)

clean:
	rm -rf ./$(WIN_BDIR)/*.o
	rm -rf ./$(LINUX_BDIR)/*.o