#ifndef __SAVED_OPT_H__
#define __SAVED_OPT_H__

typedef struct SAVEDOPTS {
	char *key;
	char *val;
	int nb_dic;
	struct SAVEDOPTS *next;
}SAVEDOPTS;

char* saved_opt_get_val(const char *option_str, const char *key);

int saved_opt_parse_option(const char *option_str, SAVEDOPTS *opts);
SAVEDOPTS* saved_opt_alloc_opts();
void saved_opt_free_opts();

#endif // !__SAVED_OPT_H__
