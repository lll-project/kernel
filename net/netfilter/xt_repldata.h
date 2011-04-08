/*
 * Today's hack: quantum tunneling in structs
 *
 * 'entries' and 'term' are never anywhere referenced by word in code. In fact,
 * they serve as the hanging-off data accessed through repl.data[].
 */

#define xt_alloc_initial_table(type, typ2) ({ \
	unsigned int hook_mask = info->valid_hooks; \
	unsigned int nhooks = hweight32(hook_mask); \
	unsigned int bytes = 0, hooknum = 0, i = 0; \
	char *tbl = kzalloc(sizeof(struct type##_replace) + \
			    __alignof__(struct type##_standard) + \
			    sizeof(struct type##_standard) * nhooks + \
			    __alignof__(struct type##_error) + \
			    sizeof(struct type##_error), GFP_KERNEL); \
	struct type##_replace *repl; \
	struct type##_standard *entries; \
	struct type##_error *term; \
	if (tbl == NULL) \
		return NULL; \
	repl = (struct type##_replace *)tbl; \
	entries = (struct type##_standard *)(tbl + sizeof(struct type##_replace) + __alignof__(struct type##_standard) - 1); \
	entries = (struct type##_standard *)((unsigned long)entries & ~(__alignof__(struct type##_standard) - 1)); \
	term = (struct type##_error *)(entries + nhooks); \
	term = (struct type##_error *)((unsigned long)term + __alignof__(struct type##_error) - 1); \
	term = (struct type##_error *)((unsigned long)term & ~(__alignof__(struct type##_error) - 1)); \
	strncpy(repl->name, info->name, sizeof(repl->name)); \
	*term = (struct type##_error)typ2##_ERROR_INIT;  \
	repl->valid_hooks = hook_mask; \
	repl->num_entries = nhooks + 1; \
	repl->size = nhooks * sizeof(struct type##_standard) + \
	                 sizeof(struct type##_error); \
	for (; hook_mask != 0; hook_mask >>= 1, ++hooknum) { \
		if (!(hook_mask & 1)) \
			continue; \
		repl->hook_entry[hooknum] = bytes; \
		repl->underflow[hooknum]  = bytes; \
		entries[i++] = (struct type##_standard) typ2##_STANDARD_INIT(NF_ACCEPT); \
		bytes += sizeof(struct type##_standard); \
	} \
	tbl; \
})
