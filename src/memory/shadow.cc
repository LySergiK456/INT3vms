#include <assert.h>
#include <type_traits>
#include "../../include/conf.hh"
#include "../../include/syx86/syx86.hh"
#include "../../include/syx86/uc.hh"

template <typename P> static int _write_value(P *addr, auto v);

namespace SlitYaX86 {
	template uc_err Ldr::mwrite<void *>(__u64 addr, void *v);
	template uc_err Ldr::mwrite<void **>(__u64 addr, void **v);
	template uc_err Ldr::mwrite<__u64>(__u64 addr, __u64 v);
	template uc_err Ldr::mwrite<long long>(__u64 addr, long long v);
	template uc_err Ldr::mwrite<__u32>(__u64 addr, __u32 v);
	template uc_err Ldr::mwrite<int>(__u64 addr, int v);
	template uc_err Ldr::mwrite<__u16>(__u64 addr, __u16 v);
	template uc_err Ldr::mwrite<short>(__u64 addr, short v);
	template uc_err Ldr::mwrite<__u8>(__u64 addr, __u8 v);
	template uc_err Ldr::mwrite<char>(__u64 addr, char v);

	template uc_err Ldr::mcopy<void *>(__u64 addr, void *v);
	template uc_err Ldr::mcopy<void **>(__u64 addr, void **v);
	template uc_err Ldr::mcopy<__u64>(__u64 addr, __u64 v);
	template uc_err Ldr::mcopy<long long>(__u64 addr, long long v);
	template uc_err Ldr::mcopy<__u32>(__u64 addr, __u32 v);
	template uc_err Ldr::mcopy<int>(__u64 addr, int v);
	template uc_err Ldr::mcopy<__u16>(__u64 addr, __u16 v);
	template uc_err Ldr::mcopy<short>(__u64 addr, short v);
	template uc_err Ldr::mcopy<__u8>(__u64 addr, __u8 v);
	template uc_err Ldr::mcopy<char>(__u64 addr, char v);
};

namespace SlitYaX86 {
uc_err Ldr::mwrite(__u64 addr, auto v) {
    uc_err err{UC_ERR_OK};
    static_assert(sizeof(v) <= sizeof(void**));
    err = uc_mem_write(uc, addr, (void *)&v, sizeof(v));
    if (!_uc_okay(err, ok))
        return err;

    auto sh_mem = VAtoShadow(addr);
    if (!sh_mem || !sh_mem->addr)
        return UC_ERR_NOMEM;

    if ((   typeid(v) == typeid(void *)
        ||  typeid(v) == typeid(void **)
        ||  typeid(v) == typeid(__u64)
        ||  typeid(v) == typeid(long long))
        &&  !!Uc::get_map(uc, *(__u64 *)&v).begin)
    {
        __u64 value{*(__u64 *)&v};

        auto v_sh_mem = VAtoShadow(value);
        if (!v_sh_mem) // case of passing map+size
            v_sh_mem = VAtoShadow(value - 1);

        if (!v_sh_mem || !v_sh_mem->addr)
            return UC_ERR_NOMEM;

        _write_value(sh_mem->ptr(addr), v_sh_mem->ptr(value));
    } else {
        _write_value(sh_mem->ptr(addr), v);
    }

    return err;
}

uc_err Ldr::mmemcpy(__u64 virt, void *ptr, __u64 size) {
    uc_err err{UC_ERR_OK};
    err = uc_mem_write(uc, virt, ptr, size);
    if (!_uc_okay(err, ok))
        return err;

    auto sh_mem = VAtoShadow(virt);
    if (!sh_mem || !sh_mem->addr)
        return UC_ERR_NOMEM;

    _write_mem(sh_mem->ptr(virt), ptr, size);

    return err;
}

uc_err Ldr::mcopy(__u64 virt, auto v) {
    uc_err err{UC_ERR_OK};
    err = uc_mem_write(uc, virt, (void *)&v, sizeof(v));
    if (!_uc_okay(err, ok))
        return err;

    auto sh_mem = VAtoShadow(virt);
    if (!sh_mem || !sh_mem->addr)
        return UC_ERR_NOMEM;

    _write_value(sh_mem->ptr(virt), v);

    return err;
}
};

template <typename P> static int _write_value(P *addr, auto v) {
    __u32 dw;

    if (!__wr_init(addr, PAGE_SIZE, &dw))
        return -1;
    do {
    	if (	typeid(v) == typeid(__u64)
    		||	typeid(v) == typeid(long long)
    		||	typeid(v) == typeid(void*)
    		||	typeid(v) == typeid(void**))
    	{
    		*(void**)addr = *(void**)&v;
    	}
    	else if (	typeid(v) == typeid(int)
    		||		typeid(v) == typeid(__u32))
    	{
    		*(int*)addr = *(int*)&v;
    	}
    	else if (	typeid(v) == typeid(short)
    		||		typeid(v) == typeid(__u16))
    	{
    		*(short*)addr = *(short*)&v;
    	}
    	else if (	typeid(v) == typeid(char)
    		||		typeid(v) == typeid(__u8))
    	{
    		*(char*)addr = *(char*)&v;
    	}
    	else
    	{
            memmove(addr, &v, sizeof(v));
    	}
    } while (0);
    if (!__wr_exit(addr, PAGE_SIZE, dw))
        return -1;

    return 0;
}
