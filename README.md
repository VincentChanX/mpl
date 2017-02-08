#mpl(my-php-library)

mpl扩展包含了很多常用的函数,如字符串的转换,正则匹配等.

##要求

* PHP 7.0 +

##编译安装

```shell
$cd /path/to/source
$/path/to/phpize
$./configure
$make && make install
```

##API

扩展提供的函数均以mpl_前缀命名

###mpl_convert_case(mixed param, int mode) *mixed*
#####对字符串,数组键名进行大小写和下划线转换

```php
<?php
//下划线转小驼峰
mpl_convert_case('my_php_library', MPL_CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE); //return myPhpLibrary
//下划线转大驼峰
mpl_convert_case('my_php_library', MPL_CONVERT_UNDERSCORE_CASE_TO_UPPER_CAMEL_CASE); //return MyPhpLibrary
//驼峰转下划线
mpl_convert_case('myPhpLibrary', MPL_CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE); //return my_php_library
//数组键名驼峰转下划线
mpl_convert_case(['myPhpLibrary'=>123,'helloWorld'=>'test'], MPL_CONVERT_UNDERSCORE_CASE_TO_LOWER_CAMEL_CASE);
//return ['my_php_library'=>123,'hello_world'=>'test']

```

###mpl_generate_random_string(int str_len, [string chars]) *string*
#####生成随机字符串

```php
<?php
mpl_generate_random_string(10, 'abc123');

```

###mpl_verify(int str, int type) *int*
#####验证字符串是否为ipv4,email

```php
<?php
mpl_verify('127.0.0.256', MPL_VERIFY_IPV4); //return 0
mpl_verify('123@test.com', MPL_VERIFY_EMAIL); //return 1

```