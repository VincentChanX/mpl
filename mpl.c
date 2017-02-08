/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mpl.h"
#include "library.h"

#include "ext/pcre/php_pcre.h"

#include <time.h>
#include <stdlib.h>

/* If you declare any globals in php_mpl.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mpl)
*/

/* True global resources - no need for thread safety here */
static int le_mpl;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mpl.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mpl_globals, mpl_globals)
    STD_PHP_INI_ENTRY("mpl.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mpl_globals, mpl_globals)
PHP_INI_END()
*/
/* }}} */


/* {{{ proto mixed mpl_convert_case(mixed param, int mode)
    对字符串、数组键名进行大小写、下划线转换 */
PHP_FUNCTION(mpl_convert_case)
{
    zval *value;
    zend_long mode = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &value, &mode) == FAILURE) {
        return;
    }

    if (mode == CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE || mode == CONVERT_UNDERSCORE_CASE_TO_UPPER_CAMEL_CASE) {
        if (Z_TYPE_P(value) == IS_STRING) {
            size_t str_len = 0;
            char *str = mpl_underscore_case_to_camel_case_for_string(Z_STRVAL_P(value), Z_STRLEN_P(value), &str_len, mode == CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE);
            RETVAL_STRINGL(str, str_len);
            efree(str);
        }
        else if (Z_TYPE_P(value) == IS_ARRAY) {
            zval result;
            array_init_size(&result, zend_hash_num_elements(Z_ARRVAL_P(value)));
            mpl_underscore_case_to_camel_case_for_array(Z_ARRVAL_P(value), &result, mode == CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE);
            RETVAL_ARR(Z_ARRVAL(result));
        }
    }
    else if (mode == CONVERT_CAMEL_CASE_TO_UNDERSCORE_CASE) {
        if (Z_TYPE_P(value) == IS_STRING) {
            size_t str_len = 0;
            char *str = mpl_camel_case_to_underscore_case_for_string(Z_STRVAL_P(value), Z_STRLEN_P(value), &str_len);
            RETVAL_STRINGL(str, str_len);
            efree(str);
        }
        else if (Z_TYPE_P(value) == IS_ARRAY) {
            zval result;
            array_init_size(&result, zend_hash_num_elements(Z_ARRVAL_P(value)));

            mpl_camel_case_to_underscore_case_for_array(Z_ARRVAL_P(value), &result);
            RETVAL_ARR(Z_ARRVAL(result));
        }
    }
}
/* }}} */


/* {{{ proto string mpl_generate_random_string(int len, [string str])
    生成随机字符串 */
PHP_FUNCTION(mpl_generate_random_string)
{
    zend_long random_str_len = 0;
    zend_string *chars = NULL;
    size_t i = 0, chars_len = 0;
    char *result = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|S", &random_str_len, &chars) == FAILURE) {
        return;
    }

    if(random_str_len <= 0) {
        RETURN_EMPTY_STRING();
    }

    if(chars == NULL) {
        chars_len = sizeof(DEFAULT_CHARS) - 1;
        chars = zend_string_init(DEFAULT_CHARS, chars_len, 0);
    }
    else {
        chars_len = ZSTR_LEN(chars);
        if (chars_len <= 0) {
            RETURN_EMPTY_STRING();
        }
    }

    result = (char *)emalloc(random_str_len);

    for(i = 0; i < random_str_len; i++) {
        result[i] = ZSTR_VAL(chars)[rand() % chars_len];
    }

    RETVAL_STRINGL(result, random_str_len);
    efree(result);
    zend_string_release(chars);
}
/* }}} */


/* {{{ proto string mpl_verify(string str, int type)
    验证字符串是否为ipv4地址、邮箱地址 */
PHP_FUNCTION(mpl_verify)
{
    zend_string *subject = NULL;
    zend_long type = 0;
    zend_string *regex = NULL;
    pcre_cache_entry *pce;
    zval *subpats = NULL;
    zend_long flags = 0;
    zend_long start_offset = 0;
    int global = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl", &subject, &type) == FAILURE) {
        return;
    }

    // php_printf("%s\r\n", ZSTR_VAL(subject));

    if (type == VERIFY_IPV4) {
        regex = zend_string_init(REGEX_IPV4, sizeof(REGEX_IPV4) - 1, 0);
    }
    else if (type == VERIFY_EMAIL) {
        regex = zend_string_init(REGEX_EMAIL, sizeof(REGEX_EMAIL) - 1, 0);
    } else {
        RETURN_FALSE;
    }

    /* Compile regex or get it from cache. */
    if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
        RETURN_FALSE;
    }

    pce->refcount++;
    php_pcre_match_impl(pce, ZSTR_VAL(subject), (int)ZSTR_LEN(subject), return_value, subpats,
        global, ZEND_NUM_ARGS() >= 4, flags, start_offset);
    pce->refcount--;

    zend_string_release(regex);
}
/* }}} */

/* {{{ php_mpl_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mpl_init_globals(zend_mpl_globals *mpl_globals)
{
	mpl_globals->global_value = 0;
	mpl_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mpl)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

    REGISTER_LONG_CONSTANT("MPL_CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE", CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("MPL_CONVERT_UNDERSCORE_CASE_TO_UPPER_CAMEL_CASE", CONVERT_UNDERSCORE_CASE_TO_UPPER_CAMEL_CASE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("MPL_CONVERT_CAMEL_CASE_TO_UNDERSCORE_CASE", CONVERT_CAMEL_CASE_TO_UNDERSCORE_CASE, CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("MPL_VERIFY_IPV4", VERIFY_IPV4, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("MPL_VERIFY_EMAIL", VERIFY_EMAIL, CONST_CS | CONST_PERSISTENT);

    srand(time(NULL));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mpl)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mpl)
{
#if defined(COMPILE_DL_MPL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mpl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mpl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mpl support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ mpl_functions[]
 *
 * Every user visible function must have an entry in mpl_functions[].
 */
const zend_function_entry mpl_functions[] = {
    PHP_FE(mpl_convert_case, NULL)
    PHP_FE(mpl_generate_random_string, NULL)
    PHP_FE(mpl_verify, NULL)
	PHP_FE_END	/* Must be the last line in mpl_functions[] */
};
/* }}} */

/* {{{ mpl_module_entry
 */
zend_module_entry mpl_module_entry = {
	STANDARD_MODULE_HEADER,
	"mpl",
	mpl_functions,
	PHP_MINIT(mpl),
	PHP_MSHUTDOWN(mpl),
	PHP_RINIT(mpl),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mpl),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mpl),
	PHP_MPL_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MPL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(mpl)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
