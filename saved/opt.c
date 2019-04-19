#include "opt.h"
#include "define.h"
#include <string.h>

char* saved_opt_get_val(const char *option_str, const char *key) {
	RETIFNULL(option_str) SAVED_E_USE_NULL;
	RETIFNULL(key) SAVED_E_USE_NULL;


}

int saved_opt_parse_option(const char *option_str, SAVEDOPTS *opts) {
	return SAVED_E_UNDEFINE;
}
SAVEDOPTS* saved_opt_alloc_opts() {
	SAVEDOPTS *ret = (SAVEDOPTS*)malloc(sizeof(SAVEDOPTS));
	RETIFNULL(ret) NULL;
	memset(ret, 0, sizeof(SAVEDOPTS));
	return ret;
}
void saved_opt_free_opts() {
}
