#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "library.h"
#include "zend.h"

char* mpl_underscore_case_to_camel_case_for_string(char *str, size_t str_len, size_t *new_str_len, zend_bool is_lower_camel_case)
{
    size_t i, j;
    zend_bool flag = 0;
    char *new_key = emalloc(str_len);

    for (i = 0,j = 0; i < str_len; i++) {
        if (IS_UNDERSCORE(str[i])) {
            flag = 1;
        }
        else {
            if (flag == 1) {
                new_key[j++] = (IS_MINUSCULE(str[i]) ? str[i] - 32 : str[i]);
                flag = 0;
            }
            else {
                new_key[j++] = str[i];
            }
        }
    }

    *new_str_len = j;
    if (is_lower_camel_case == 0 && str_len > 0 && IS_MINUSCULE(new_key[0])) {
        new_key[0] -= 32;
    }
    return new_key;
}


void mpl_underscore_case_to_camel_case_for_array(HashTable *ht, zval *result, zend_bool is_lower_camel_case)
{
    zend_string *key = NULL;
    zval *data = NULL;
    zend_ulong index = 0;

    char *new_key = NULL;
    size_t new_key_len = 0;
    zval *tmp_data;

    ZEND_HASH_FOREACH_KEY_VAL_IND(ht, index, key, data) {

        // if(key) {
        //     php_printf("key=");
        //     PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
        // } else {
        //     php_printf("index=%d", index);
        // }
        // php_printf(" data type = %d\r\n", Z_TYPE_P(data));

        if (Z_TYPE_P(data) == IS_ARRAY) {
            zval tmp_arr;
            array_init_size(&tmp_arr, zend_hash_num_elements(Z_ARRVAL_P(data)));
            mpl_underscore_case_to_camel_case_for_array(Z_ARRVAL_P(data), &tmp_arr, is_lower_camel_case);
            tmp_data = &tmp_arr;
        }
        else {
            tmp_data = data;
            //add ref
            zval_add_ref(data);
        }

        if (key != NULL) {
            new_key = mpl_underscore_case_to_camel_case_for_string(ZSTR_VAL(key), ZSTR_LEN(key), &new_key_len, is_lower_camel_case);
            add_assoc_zval_ex(result, new_key, new_key_len, tmp_data);
            efree(new_key);
        }
        else {
            add_index_zval(result, index, tmp_data);
        }

    } ZEND_HASH_FOREACH_END();
}


char* mpl_camel_case_to_underscore_case_for_string(char *str, size_t str_len, size_t *new_str_len)
{
    size_t i, j;
    zend_bool flag = 0;
    char * result = NULL;
    *new_str_len = str_len;
    for (i = 1; i < str_len; i++) {
        if (IS_CAPITAL(str[i])) {
            (*new_str_len)++;
        }
    }
    result = (char *)emalloc(*new_str_len);
    if (str_len > 0) {
        result[0] = str[0];

        for (i = 1,j = 1; i < str_len; i++) {
            if (IS_CAPITAL(str[i])) {
                result[j++] = '_';
                result[j++] = str[i] + 32;
            }
            else {
                result[j++] = str[i];
            }
        }
    }
    return result;
}



void mpl_camel_case_to_underscore_case_for_array(HashTable *ht, zval *result)
{
    zend_string *key = NULL;
    zval *data = NULL;
    zend_ulong index = 0;

    char *new_key = NULL;
    size_t new_key_len = 0;
    zval *tmp_data;

    ZEND_HASH_FOREACH_KEY_VAL_IND(ht, index, key, data) {

        if (Z_TYPE_P(data) == IS_ARRAY) {
            zval tmp_arr;
            array_init_size(&tmp_arr, zend_hash_num_elements(Z_ARRVAL_P(data)));
            mpl_camel_case_to_underscore_case_for_array(Z_ARRVAL_P(data), &tmp_arr);
            tmp_data = &tmp_arr;
        }
        else {
            tmp_data = data;
            //add ref
            zval_add_ref(data);
        }

        if (key != NULL) {
            new_key = mpl_camel_case_to_underscore_case_for_string(ZSTR_VAL(key), ZSTR_LEN(key), &new_key_len);
            add_assoc_zval_ex(result, new_key, new_key_len, tmp_data);
            efree(new_key);
        }
        else {
            add_index_zval(result, index, tmp_data);
        }

    } ZEND_HASH_FOREACH_END();
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
