/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@omniti.com>                                 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_swish.h"
#include "zend_exceptions.h"
#include "swish-e.h"

static zend_class_entry
	*ce_sw_exception,
	*ce_sw_handle,
	*ce_sw_search,
	*ce_sw_results,
	*ce_sw_result,
	*ce_sw_fuzzyword;

struct php_sw_handle {
	zend_object std;
	SW_HANDLE h;
};

struct php_sw_search {
	zend_object std;
	zval refhandle;
	struct php_sw_handle *h;
	SW_SEARCH s;
};

struct php_sw_results {
	zend_object std;
	zval refhandle;
	SW_RESULTS r;
};

struct php_sw_result {
	zend_object std;
	zval refhandle;
	SW_RESULT r;
};

static int sw_throw_exception(struct php_sw_handle *h TSRMLS_CC)
{
	if (SwishError(h->h)) {
		char *msg = SwishLastErrorMsg(h->h);
		if (!msg) {
			msg = SwishErrorString(h->h);
		}
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, msg);
		return 1;
	}
	return 0;
}

/* {{{ proto void SwishHandle::__construct(string indices)
   Open a swish index */
static PHP_METHOD(SwishHandle, __construct)
{
	zval *object = getThis();
	struct php_sw_handle *h;
	char *indices;
	int indices_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"s", &indices, &indices_len)) {
		ZVAL_NULL(object);
		return;
	}

	h = (struct php_sw_handle*)zend_object_store_get_object(object TSRMLS_CC);

	h->h = SwishInit(indices);

	if (sw_throw_exception(h TSRMLS_CC)) {
		ZVAL_NULL(object);
		return;
	}
}
/* }}} */

/* {{{ proto object SwishHandle::prepare([string query])
   Returns a new search object */
static PHP_METHOD(SwishHandle, prepare)
{
	struct php_sw_handle *h;
	char *query = NULL;
	int qlen;
	struct php_sw_search *s;
	SW_SEARCH search;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"|s", &query, &qlen)) {
		return;
	}

	h = (struct php_sw_handle*)zend_object_store_get_object(getThis() TSRMLS_CC);

	search = New_Search_Object(h->h, query);

	if (sw_throw_exception(h TSRMLS_CC)) {
		if (search) {
			Free_Search_Object(search);
		}
		return;
	}
	
	Z_TYPE_P(return_value) = IS_OBJECT;
	object_init_ex(return_value, ce_sw_search);
	return_value->refcount = 1;
	return_value->is_ref = 1;

	s = zend_object_store_get_object(return_value TSRMLS_CC);
	s->s = search;

	s->refhandle = *getThis();
	zend_objects_store_add_ref(getThis() TSRMLS_CC);
	s->h = h;
}
/* }}} */


/* {{{ proto object SwishHandle::query(string query)
   Executes a search, returning the result object */   
static PHP_METHOD(SwishHandle, query)
{
	char *query = NULL;
	int qlen;
	struct php_sw_handle *h;
	struct php_sw_results *r;
	SW_RESULTS results;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"s", &query, &qlen)) {
		return;
	}

	h = (struct php_sw_handle*)zend_object_store_get_object(getThis() TSRMLS_CC);

	results = SwishQuery(h->h, query);

	if (sw_throw_exception(h TSRMLS_CC)) {
		if (results) {
			Free_Results_Object(results);
		}
		return;
	}

	Z_TYPE_P(return_value) = IS_OBJECT;
	object_init_ex(return_value, ce_sw_results);
	return_value->refcount = 1;
	return_value->is_ref = 1;
	r = zend_object_store_get_object(return_value TSRMLS_CC);
	r->r = results;

	r->refhandle = *getThis();
	zend_objects_store_add_ref(&r->refhandle TSRMLS_CC);
}
/* }}} */


static zend_function_entry sw_handle_methods[] = {
	PHP_ME(SwishHandle, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishHandle, prepare, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishHandle, query, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void SwishSearch::setStructure(int structure)
   Sets the "structure" flag in the search object */
static PHP_METHOD(SwishSearch, setStructure)
{
	struct php_sw_search *s;
	long l;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"l", &l)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishSetStructure(s->s, l);
}
/* }}} */

/* {{{ proto void SwishSearch::setPhraseDelimiter(string delim)
   Sets the phrase delimiter character, the default is double-quotes. */
static PHP_METHOD(SwishSearch, setPhraseDelimiter)
{
	struct php_sw_search *s;
	char *str;
	int l;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"s", &str, &l)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishPhraseDelimiter(s->s, *str);
}
/* }}} */

/* {{{ proto void SwishSearch::setSort(string sort)
   Sets the sort order of the results */
static PHP_METHOD(SwishSearch, setSort)
{
	struct php_sw_search *s;
	char *str;
	int l;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"s", &str, &l)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishSetSort(s->s, str);
}
/* }}} */

/* {{{ proto void SwishSearch::setSearchLimit(string prop, string low, string hi)
   Sets the limit parameters for a search */
static PHP_METHOD(SwishSearch, setSearchLimit)
{
	struct php_sw_search *s;
	char *prop, *low, *hi;
	int pl, ll, hl;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"sss", &prop, &pl, &low, &ll, &hi, &hl)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishSetSearchLimit(s->s, prop, low, hi);
}
/* }}} */

/* {{{ proto void SwishSearch::resetSearchLimit()
   resets the limit parameters for a search */
static PHP_METHOD(SwishSearch, resetSearchLimit)
{
	struct php_sw_search *s;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"")) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishResetSearchLimit(s->s);
}
/* }}} */



/* {{{ proto object SwishSearch::execute([string query])
   Returns a new search object */
static PHP_METHOD(SwishSearch, execute)
{
	char *query = NULL;
	int qlen;
	struct php_sw_search *s;
	struct php_sw_results *r;
	SW_RESULTS results;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"|s", &query, &qlen)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);

	results = SwishExecute(s->s, query);

	if (sw_throw_exception(s->h TSRMLS_CC)) {
		if (results) {
			Free_Results_Object(results);
		}
		return;
	}

	Z_TYPE_P(return_value) = IS_OBJECT;
	object_init_ex(return_value, ce_sw_results);
	return_value->refcount = 1;
	return_value->is_ref = 1;
	r = zend_object_store_get_object(return_value TSRMLS_CC);
	r->r = results;

	r->refhandle = s->refhandle;
	zend_objects_store_add_ref(&s->refhandle TSRMLS_CC);
}
/* }}} */


static zend_function_entry sw_search_methods[] = {
	PHP_ME(SwishSearch, execute, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setStructure, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setPhraseDelimiter, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setSort, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setSearchLimit, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, resetSearchLimit, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void SwishResults::getHits()
   Returns the number of results in the results object. */
static PHP_METHOD(SwishResults, getHits)
{
	struct php_sw_results *r;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"")) {
		return;
	}

	r = (struct php_sw_results*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (!r->r) {
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, "no more results");
		return;
	}
	RETURN_LONG(SwishHits(r->r));
}
/* }}} */

/* {{{ proto long SwishResults::seekResult(long position)
   Sets the current seek position in the list of results. 0 is the first. Returns negative number on error */
static PHP_METHOD(SwishResults, seekResult)
{
	struct php_sw_results *r;
	long l;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"l", &l)) {
		return;
	}

	r = (struct php_sw_results*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (!r->r) {
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, "no more results");
		return;
	}

	RETURN_LONG(SwishSeekResult(r->r, l));
}
/* }}} */

/* {{{ proto object SwishResults::nextResult()
   Advances to the next result.  Returns false when there are no more results. */
static PHP_METHOD(SwishResults, nextResult)
{
	struct php_sw_results *r;
	struct php_sw_result *result;
	long l;
	SW_RESULT res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"")) {
		return;
	}

	r = (struct php_sw_results*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (!r->r) {
		RETURN_FALSE;
	}
	res = SwishNextResult(r->r);
	if (!res) {
		RETURN_FALSE;
	}

	Z_TYPE_P(return_value) = IS_OBJECT;
	object_init_ex(return_value, ce_sw_result);
	return_value->refcount = 1;
	return_value->is_ref = 1;
	result = zend_object_store_get_object(return_value TSRMLS_CC);
	result->r = res;

	result->refhandle = r->refhandle;
	zend_objects_store_add_ref(&r->refhandle TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed SwishResult::property(string prop)
   Returns the value of a property */
static PHP_METHOD(SwishResult, property)
{
	struct php_sw_result *r;
	char *propname;
	int l;
	PropValue *v;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"s", &propname, &l)) {
		return;
	}

	r = (struct php_sw_result*)zend_object_store_get_object(getThis() TSRMLS_CC);
	v = getResultPropValue(r->r, propname, 0);
	if (!v) {
		/* FIXME: return error */
		RETURN_FALSE;
	}

	switch(v->datatype) {
		case PROP_STRING:
			RETVAL_STRING(v->value.v_str, 1);
			break;
		case PROP_INTEGER:
			RETVAL_LONG(v->value.v_int);
			break;
		case PROP_DATE:
			RETVAL_LONG(v->value.v_date);
			break;
		case PROP_FLOAT:
			RETVAL_DOUBLE(v->value.v_float);
			break;
		case PROP_ULONG:
			RETVAL_LONG(v->value.v_ulong);
			break;
	}
	freeResultPropValue(v);
}
/* }}} */



static zend_function_entry sw_results_methods[] = {
	PHP_ME(SwishResults, getHits, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishResults, seekResult, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishResults, nextResult, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry sw_result_methods[] = {
	PHP_ME(SwishResult, property, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_object_handlers cannot_be_cloned;

/* {{{ swish_functions[] */
static zend_function_entry swish_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ swish_module_entry */
zend_module_entry swish_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"swish",
	swish_functions,
	PHP_MINIT(swish),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(swish),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1",
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SWISH
ZEND_GET_MODULE(swish)
#endif

/* note: must run on PHP < 5.1.3 */
static void obj_init(zend_object *o, zend_class_entry *ce TSRMLS_DC)
{
	zval *tmp;

	ALLOC_HASHTABLE(o->properties);
	zend_hash_init(o->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(o->properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref,
				(void*)&tmp, sizeof(zval*));
	o->ce =ce;
	o->guards = NULL;
}
static void obj_fini(zend_object *o TSRMLS_DC)
{
	if (o->properties) {
		zend_hash_destroy(o->properties);
		efree(o->properties);
		o->properties = NULL;
	}
}

static void sw_handle_dtor(void *object TSRMLS_DC)
{
	struct php_sw_handle *h = (struct php_sw_handle*)object;

	if (h->h) {
		SwishClose(h->h);
		h->h = NULL;
	}
	
	obj_fini(&h->std);

	efree(h);
}

static void sw_search_dtor(void *object TSRMLS_DC)
{
	struct php_sw_search *s = (struct php_sw_search*)object;

	if (s->s) {
		Free_Search_Object(s->s);
		s->s = NULL;
	}

	zend_objects_store_del_ref(&s->refhandle TSRMLS_CC);	
	
	obj_fini(&s->std);
	efree(s);
}

static void sw_results_dtor(void *object TSRMLS_DC)
{
	struct php_sw_results *r = (struct php_sw_results*)object;

	if (r->r) {
		Free_Results_Object(r->r);
		r->r = NULL;
	}

	zend_objects_store_del_ref(&r->refhandle TSRMLS_CC);	
	
	obj_fini(&r->std);
	efree(r);
}

static void sw_result_dtor(void *object TSRMLS_DC)
{
	struct php_sw_result *r = (struct php_sw_result*)object;

	zend_objects_store_del_ref(&r->refhandle TSRMLS_CC);	
	obj_fini(&r->std);
	efree(r);
}

static zend_object_value sw_handle_new(zend_class_entry *ce TSRMLS_DC)
{
	struct php_sw_handle *h;
	zend_object_value retval;
	zval *tmp;

	h = ecalloc(1, sizeof(*h));
	obj_init(&h->std, ce);
	retval.handle = zend_objects_store_put(h,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_handle_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &cannot_be_cloned;
	return retval;
}

static zend_object_value sw_search_new(zend_class_entry *ce TSRMLS_DC)
{
	struct php_sw_search *s;
	zend_object_value retval;
	zval *tmp;

	s = ecalloc(1, sizeof(*s));
	obj_init(&s->std, ce);

	retval.handle = zend_objects_store_put(s,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_search_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &cannot_be_cloned;
	return retval;
}

static zend_object_value sw_results_new(zend_class_entry *ce TSRMLS_DC)
{
	struct php_sw_results *r;
	zend_object_value retval;
	zval *tmp;

	r = ecalloc(1, sizeof(*r));
	obj_init(&r->std, ce);

	retval.handle = zend_objects_store_put(r,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_results_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &cannot_be_cloned;
	return retval;
}

static zend_object_value sw_result_new(zend_class_entry *ce TSRMLS_DC)
{
	struct php_sw_result *r;
	zend_object_value retval;
	zval *tmp;

	r = ecalloc(1, sizeof(*r));
	obj_init(&r->std, ce);

	retval.handle = zend_objects_store_put(r,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_result_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &cannot_be_cloned;
	return retval;
}
				
/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(swish)
{
	zend_class_entry ce;

	memcpy(&cannot_be_cloned, zend_get_std_object_handlers(), sizeof(cannot_be_cloned));
	cannot_be_cloned.clone_obj = NULL;
	
	INIT_CLASS_ENTRY(ce, "SwishException", NULL);
	ce_sw_exception = zend_register_internal_class_ex(&ce, 
				zend_exception_get_default(), NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "SwishHandle", sw_handle_methods);
	ce_sw_handle = zend_register_internal_class(&ce TSRMLS_CC);
	ce_sw_handle->create_object = sw_handle_new;

	INIT_CLASS_ENTRY(ce, "SwishSearch", sw_search_methods);
	ce_sw_search = zend_register_internal_class(&ce TSRMLS_CC);
	ce_sw_search->create_object = sw_search_new;

	INIT_CLASS_ENTRY(ce, "SwishResults", sw_results_methods);
	ce_sw_results = zend_register_internal_class(&ce TSRMLS_CC);
	ce_sw_results->create_object = sw_results_new;

	INIT_CLASS_ENTRY(ce, "SwishResult", sw_result_methods);
	ce_sw_result = zend_register_internal_class(&ce TSRMLS_CC);
	ce_sw_result->create_object = sw_result_new;

	REGISTER_LONG_CONSTANT("SWISH_META_TYPE_UNDEF", SW_META_TYPE_UNDEF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_META_TYPE_STRING", SW_META_TYPE_STRING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_META_TYPE_ULONG", SW_META_TYPE_ULONG, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_META_TYPE_DATE", SW_META_TYPE_DATE, CONST_CS|CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("SWISH_IN_FILE_BIT", IN_FILE_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_TITLE_BIT", IN_TITLE_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_HEAD_BIT", IN_HEAD_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_BODY_BIT", IN_BODY_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_COMMENTS_BIT", IN_COMMENTS_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_HEADER_BIT", IN_HEADER_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_EMPHASIZED_BIT", IN_EMPHASIZED_BIT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_META_BIT", IN_META_BIT, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SWISH_IN_FILE", IN_FILE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_TITLE", IN_TITLE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_HEAD", IN_HEAD, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_BODY", IN_BODY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_COMMENTS", IN_COMMENTS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_HEADER", IN_HEADER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_EMPHASIZED", IN_EMPHASIZED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SWISH_IN_META", IN_META, CONST_CS|CONST_PERSISTENT);
		
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(swish)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "swish support", "enabled");
	/* DO NOT REMOVE THIS URL!
	 * It is here for license compliance */
	php_info_print_table_row(2, "source available from", "http://swish-e.org");
	php_info_print_table_end();
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
