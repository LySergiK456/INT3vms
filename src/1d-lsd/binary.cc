SlitYaX86::Binary_t::Binary_t(int type, void *map, __u64 sz)
    : t{type}, src{ .map=map }, size{sz}
{
    llist_init(&ll_regions, sizeof(struct load_region));
}