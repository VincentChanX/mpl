#ifndef MPL_LIBRARY_H
#define MPL_LIBRARY_H

#include "zend.h"

//判断字符是否为大写字母
#define IS_CAPITAL(c) (c >= 'A' && c <= 'Z')
//判断字符是否为下划线
#define IS_UNDERSCORE(c) (c == '_')
//判断字符是否为小写字母
#define IS_MINUSCULE(c) (c >= 'a' && c <= 'z')

//下划线风格转换为小驼峰风格
#define CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE 1
//下划线风格转换为大驼峰风格
#define CONVERT_UNDERSCORE_CASE_TO_UPPER_CAMEL_CASE 2
//驼峰风格转换为下划线风格
#define CONVERT_CAMEL_CASE_TO_UNDERSCORE_CASE 3

#define DEFAULT_CHARS "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"

#define REGEX_IPV4 "/^(((\\d{1,2})|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))\\.){3}((\\d{1,2})|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))$/"
#define REGEX_EMAIL "/^\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*$/"

#define VERIFY_IPV4 1
#define VERIFY_EMAIL 2


char* mpl_underscore_case_to_camel_case_for_string(char *str, size_t str_len, size_t *new_str_len, zend_bool is_lower_camel_case);
void mpl_underscore_case_to_camel_case_for_array(HashTable *ht, zval *result, zend_bool is_lower_camel_case);
char* mpl_camel_case_to_underscore_case_for_string(char *str, size_t str_len, size_t *new_str_len);
void mpl_camel_case_to_underscore_case_for_array(HashTable *ht, zval *result);

#endif                          /* MPL_LIBRARY_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
