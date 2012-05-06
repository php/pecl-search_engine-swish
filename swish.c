/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Wez Furlong <wez@omniti.com>                                |
  |          Antony Dovgal <tony2001@php.net>                            |
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

/* {{{ internal structs */
static zend_class_entry *ce_sw_exception; 
static zend_class_entry *ce_sw_handle;
static zend_class_entry *ce_sw_search;
static zend_class_entry *ce_sw_result; 
static zend_class_entry *ce_sw_results;

static zend_object_handlers php_sw_results_obj_handlers;
static zend_object_handlers php_sw_result_obj_handlers;
static zend_object_handlers php_sw_handle_obj_handlers;
static zend_object_handlers cannot_be_cloned;

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
	struct php_sw_handle *h;
	SW_RESULTS r;
};

struct php_sw_result {
	zend_object std;
	zval refhandle;
	struct php_sw_handle *h;
	SW_RESULT r;
};
/* }}} */

#if PHP_VERSION_ID < 50300
# define Z_SET_REFCOUNT_P(pz, rc) pz->refcount = rc
# define Z_SET_ISREF_P(pz)        pz->is_ref = 1
# define Z_UNSET_ISREF_P(pz)      pz->is_ref = 0
#endif

/* {{{ internal functions */
static int sw_throw_exception(struct php_sw_handle *h TSRMLS_DC) /* {{{ */
{
	if (SwishError(h->h)) {
		char *msg = SwishLastErrorMsg(h->h);
		if (!msg || *msg == 0 ) {
			msg = SwishErrorString(h->h);
		}
		if (!msg || *msg == 0) {
			/* this usually should not happen */
			msg = "Unknown error occured. Please report";
		}
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, msg);
		return 1;
	}
	return 0;
}
/* }}} */

#ifndef HAVE_STEMMER_CONSTANTS /* {{{ */
/* if these constants were not exported, define them */
enum {
	STEM_OK,
	STEM_NOT_ALPHA,
	STEM_TOO_SMALL,
	STEM_WORD_TOO_BIG,
	STEM_TO_NOTHING
};
#endif
/* }}} */

static int sw_stemmer_error_to_exception(int error TSRMLS_DC) /* {{{ */
{
	if (error != STEM_OK) {
		char *msg;
		switch(error) {
			case STEM_NOT_ALPHA:
				msg = "Not all letters are alpha";
				break;
			case STEM_TOO_SMALL:
				msg = "The word is too small to be stemmed";
				break;
			case STEM_WORD_TOO_BIG:
				msg = "The word is too big to be stemmed";
				break;
			case STEM_TO_NOTHING:
				msg = "The word was stemmed to empty string";
				break;
			default:
				msg = "Unknown stemming error";
				break;
		}
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, msg);
		return 1;
	}
	return 0;
}
/* }}} */

static void php_sw_header_to_zval(SWISH_HEADER_VALUE value, SWISH_HEADER_TYPE type, zval** retval, zend_bool alloc_zval TSRMLS_DC) /* {{{ */
{
	if (alloc_zval) {
		MAKE_STD_ZVAL(*retval);
	}

	switch (type) {
		case SWISH_STRING:
			ZVAL_STRING(*retval, (char *)value.string, 1);
			break;

		case SWISH_NUMBER:
			ZVAL_LONG(*retval, value.number);
			break;

		case SWISH_BOOL:
			ZVAL_BOOL(*retval, value.boolean);
			return;

		case SWISH_LIST: 
			{
				const char **string_list;
				string_list = value.string_list;
				array_init(*retval);

				while ( string_list && *string_list )
				{
					add_next_index_string(*retval, (char *)(*string_list), 1);
					string_list++;
				}
			}
			break;

		case SWISH_HEADER_ERROR:
			ZVAL_NULL(*retval);
			break;

		default:
			ZVAL_NULL(*retval);
			break;
	}
}
/* }}} */

static void sw_handle_dtor(void *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_handle *h = (struct php_sw_handle*)object;

	if (h->h) {
		SwishClose(h->h);
		h->h = NULL;
	}

	zend_object_std_dtor(&h->std TSRMLS_CC);
	efree(h);
}
/* }}} */

static void sw_search_dtor(void *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_search *s = (struct php_sw_search*)object;

	if (s->s) {
		Free_Search_Object(s->s);
		s->s = NULL;
	}

	zend_objects_store_del_ref(&s->refhandle TSRMLS_CC);	

	zend_object_std_dtor(&s->std TSRMLS_CC);
	efree(s);
}
/* }}} */

static void sw_results_dtor(void *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_results *r = (struct php_sw_results*)object;

	if (r->r) {
		Free_Results_Object(r->r);
		r->r = NULL;
	}

	zend_objects_store_del_ref(&r->refhandle TSRMLS_CC);	

	zend_object_std_dtor(&r->std TSRMLS_CC);
	efree(r);
}
/* }}} */

static void sw_result_dtor(void *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_result *r = (struct php_sw_result*)object;

	zend_objects_store_del_ref(&r->refhandle TSRMLS_CC);	

	zend_object_std_dtor(&r->std TSRMLS_CC);
	efree(r);
}
/* }}} */

static zend_object_value sw_handle_new(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	struct php_sw_handle *h;
	zend_object_value retval;

	h = ecalloc(1, sizeof(*h));
	zend_object_std_init(&h->std, ce TSRMLS_CC);

	retval.handle = zend_objects_store_put(h,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_handle_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &php_sw_handle_obj_handlers;
	return retval;
}
/* }}} */

static zend_object_value sw_search_new(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	struct php_sw_search *s;
	zend_object_value retval;

	s = ecalloc(1, sizeof(*s));
	zend_object_std_init(&s->std, ce TSRMLS_CC);

	retval.handle = zend_objects_store_put(s,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_search_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &cannot_be_cloned;
	return retval;
}
/* }}} */

static zend_object_value sw_results_new(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	struct php_sw_results *r;
	zend_object_value retval;

	r = ecalloc(1, sizeof(*r));
	zend_object_std_init(&r->std, ce TSRMLS_CC);

	retval.handle = zend_objects_store_put(r,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_results_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &php_sw_results_obj_handlers;
	return retval;
}
/* }}} */

static zend_object_value sw_result_new(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	struct php_sw_result *r;
	zend_object_value retval;

	r = ecalloc(1, sizeof(*r));
	zend_object_std_init(&r->std, ce TSRMLS_CC);

	retval.handle = zend_objects_store_put(r,
			(zend_objects_store_dtor_t)zend_objects_destroy_object,
			sw_result_dtor,
			NULL TSRMLS_CC);
	retval.handlers = &php_sw_result_obj_handlers;
	return retval;
}
/* }}} */

static void php_sw_results_indexes_to_array(struct php_sw_results *r, zval **z_indexes TSRMLS_DC) /* {{{ */
{
	zval *index, *ztmp, *element;
	const char **index_names;
	SWISH_HEADER_VALUE header_value;
	SWISH_HEADER_TYPE  header_type;

	index_names = SwishIndexNames(r->h->h);

	MAKE_STD_ZVAL(*z_indexes);
	array_init(*z_indexes);

	while (index_names && *index_names) {
		MAKE_STD_ZVAL(index);
		array_init(index);

		MAKE_STD_ZVAL(ztmp);
		ZVAL_STRING(ztmp, (char *)(*index_names), 1);

		add_assoc_zval(index, "name", ztmp);

		header_type = SWISH_LIST;
		header_value = SwishParsedWords(r->r, *index_names);
		php_sw_header_to_zval(header_value, header_type, &element, 1 TSRMLS_CC);
		add_assoc_zval(index, "parsed_words", element);

		header_type = SWISH_LIST;
		header_value = SwishRemovedStopwords(r->r, *index_names);
		php_sw_header_to_zval(header_value, header_type, &element, 1 TSRMLS_CC);
		add_assoc_zval(index, "removed_stop_words", element);

		add_next_index_zval(*z_indexes, index);
		index_names++;
	}
}
/* }}} */

#if PHP_VERSION_ID >= 50400
static zval *php_sw_results_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
#else
static zval *php_sw_results_read_property(zval *object, zval *member, int type TSRMLS_DC)
#endif
{
	struct php_sw_results *r;
	zval tmp_member;
	zval *retval;
	zend_object_handlers *std_hnd;

	r = (struct php_sw_results*)zend_object_store_get_object(object TSRMLS_CC);

	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (Z_STRLEN_P(member) == (sizeof("hits") - 1) && !memcmp(Z_STRVAL_P(member), "hits", Z_STRLEN_P(member))){
		MAKE_STD_ZVAL(retval);
		ZVAL_LONG(retval, SwishHits(r->r));
		Z_SET_REFCOUNT_P(retval, 0);
	} else if (Z_STRLEN_P(member) == (sizeof("indexes") - 1) && !memcmp(Z_STRVAL_P(member), "indexes", Z_STRLEN_P(member))) {
		php_sw_results_indexes_to_array(r, &retval TSRMLS_CC);
		Z_SET_REFCOUNT_P(retval, 0);
	} else {
		std_hnd = zend_get_std_object_handlers();
#if PHP_VERSION_ID >= 50400
		retval = std_hnd->read_property(object, member, type, key TSRMLS_CC);
#else
		retval = std_hnd->read_property(object, member, type TSRMLS_CC);
#endif

	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return(retval);
}
/* }}} */

static HashTable *php_sw_results_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_results *r;
	zval *tmp;

	r = (struct php_sw_results*)zend_objects_get_address(object TSRMLS_CC);

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, SwishHits(r->r));

	zend_hash_update(r->std.properties, "hits", sizeof("hits"), (void *)&tmp, sizeof(zval *), NULL);

	php_sw_results_indexes_to_array(r, &tmp TSRMLS_CC);
	zend_hash_update(r->std.properties, "indexes", sizeof("indexes"), (void *)&tmp, sizeof(zval *), NULL);
	return r->std.properties;
}
/* }}} */

static void php_sw_prop_to_zval(struct php_sw_result *r, char *name , zval **prop TSRMLS_DC) /* {{{ */
{
	PropValue *v;

	MAKE_STD_ZVAL(*prop);

	v = getResultPropValue(r->r, name, 0);
	if (!v) {
		ZVAL_NULL(*prop);
	} else {
		switch(v->datatype) {
			case PROP_STRING:
				ZVAL_STRING(*prop, v->value.v_str, 1);
				break;
			case PROP_INTEGER:
				ZVAL_LONG(*prop, v->value.v_int);
				break;
			case PROP_DATE:
				ZVAL_LONG(*prop, v->value.v_date);
				break;
			case PROP_FLOAT:
				ZVAL_DOUBLE(*prop, v->value.v_float);
				break;
			case PROP_ULONG:
				ZVAL_LONG(*prop, v->value.v_ulong);
				break;
			default:
				ZVAL_NULL(*prop);
				break;
		}
		freeResultPropValue(v);
	}
}
/* }}}  */

#if PHP_VERSION_ID >= 50400
static zval *php_sw_result_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
#else
static zval *php_sw_result_read_property(zval *object, zval *member, int type TSRMLS_DC)
#endif
{
	struct php_sw_result *r;
	zval tmp_member;
	zval *retval;
	zend_object_handlers *std_hnd;

	r = (struct php_sw_result*)zend_object_store_get_object(object TSRMLS_CC);

	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	php_sw_prop_to_zval(r, Z_STRVAL_P(member), &retval TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_NULL) {
		/* not found */
		zval_ptr_dtor(&retval);
		std_hnd = zend_get_std_object_handlers();
#if PHP_VERSION_ID >= 50400
		retval = std_hnd->read_property(object, member, type, key TSRMLS_CC);
#else
		retval = std_hnd->read_property(object, member, type TSRMLS_CC);
#endif
	} else {
		Z_SET_REFCOUNT_P(retval, 0);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return(retval);
}
/* }}} */

static HashTable *php_sw_result_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_result *r;
	zval *prop;
	SWISH_META_LIST meta;
	char *name;

	r = (struct php_sw_result*)zend_objects_get_address(object TSRMLS_CC);

	meta = SwishResultPropertyList(r->r);

	while (meta && *meta) {
		name = (char*)SwishMetaName(*meta);
		php_sw_prop_to_zval(r, name, &prop TSRMLS_CC);
		zend_hash_update(r->std.properties, name, strlen(name)+1, (void *)&prop, sizeof(zval *), NULL);
		meta++;
	}

	return r->std.properties;
}
/* }}} */

static void php_sw_handle_indexes_to_array(struct php_sw_handle *h, zval **z_indexes TSRMLS_DC) /* {{{ */
{
	zval *index, *ztmp, *header;
	const char **index_names, **header_names, **curr_header;
	SWISH_HEADER_VALUE header_value;
	SWISH_HEADER_TYPE  header_type;

	index_names = SwishIndexNames(h->h);
	header_names = SwishHeaderNames(h->h);

	MAKE_STD_ZVAL(*z_indexes);
	array_init(*z_indexes);

	while (index_names && *index_names) {
		MAKE_STD_ZVAL(index);
		array_init(index);

		MAKE_STD_ZVAL(ztmp);
		ZVAL_STRING(ztmp, (char *)(*index_names), 1);

		add_assoc_zval(index, "name", ztmp);

		MAKE_STD_ZVAL(ztmp);
		array_init(ztmp);

		curr_header = header_names;
		while (curr_header && *curr_header) {
			header_value = SwishHeaderValue(h->h, *index_names, *curr_header, &header_type);
			php_sw_header_to_zval(header_value, header_type, &header, 1 TSRMLS_CC);
			add_assoc_zval(ztmp, (char *)(*curr_header), header);

			curr_header++;
		}

		add_assoc_zval(index, "headers", ztmp);
		add_next_index_zval(*z_indexes, index);
		index_names++;
	}
}
/* }}} */

#if PHP_VERSION_ID >= 50400
static zval *php_sw_handle_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
#else
static zval *php_sw_handle_read_property(zval *object, zval *member, int type TSRMLS_DC)
#endif
{
	struct php_sw_handle *h;
	zval tmp_member;
	zval *retval;
	zend_object_handlers *std_hnd;

	h = (struct php_sw_handle*)zend_object_store_get_object(object TSRMLS_CC);

	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (Z_STRLEN_P(member) == (sizeof("indexes") - 1) && !memcmp(Z_STRVAL_P(member), "indexes", Z_STRLEN_P(member))){
		php_sw_handle_indexes_to_array(h, &retval TSRMLS_CC);
		Z_SET_REFCOUNT_P(retval, 0);
	} else {
		std_hnd = zend_get_std_object_handlers();
#if PHP_VERSION_ID >= 50400
		retval = std_hnd->read_property(object, member, type, key TSRMLS_CC);
#else
		retval = std_hnd->read_property(object, member, type TSRMLS_CC);
#endif
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}
/* }}} */

static HashTable *php_sw_handle_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	struct php_sw_handle *h;
	zval *z_indexes;

	h = (struct php_sw_handle*)zend_objects_get_address(object TSRMLS_CC);

	php_sw_handle_indexes_to_array(h, &z_indexes TSRMLS_CC);
	zend_hash_update(h->std.properties, "indexes", sizeof("indexes"), (void *)&z_indexes, sizeof(zval *), NULL);

	return h->std.properties;
}
/* }}} */

static void fill_property_list(zval *return_value, SWISH_META_LIST meta TSRMLS_DC) /* {{{ */
{
	zval *prop;

	array_init(return_value);

	while (meta && *meta) {
		MAKE_STD_ZVAL(prop);
		array_init(prop);

		add_assoc_string(prop, "Name", (char*)SwishMetaName(*meta), 1);
		add_assoc_long(prop, "Type", SwishMetaType(*meta));
		add_assoc_long(prop, "ID", SwishMetaID(*meta));

		add_next_index_zval(return_value, prop);
		meta++;
	}
}
/* }}} */

/* }}} */

/* {{{ proto void Swish::__construct(string indices)
   Open a swish index */
static PHP_METHOD(Swish, __construct)
{
	zval *object = getThis();
	struct php_sw_handle *h;
	char *indices;
	int indices_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &indices, &indices_len)) {
		return;
	}

	h = (struct php_sw_handle*)zend_object_store_get_object(object TSRMLS_CC);

	if (h->h) {
		/* called __construct() twice, bail out */
		return;
	}

	h->h = SwishInit(indices);

	if (sw_throw_exception(h TSRMLS_CC)) {
		return;
	}
}
/* }}} */

/* {{{ proto object Swish::prepare([string query])
   Returns a new search object */
static PHP_METHOD(Swish, prepare)
{
	struct php_sw_handle *h;
	char *query = NULL;
	int qlen;
	struct php_sw_search *s;
	SW_SEARCH search;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &query, &qlen)) {
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

	object_init_ex(return_value, ce_sw_search);
	Z_SET_REFCOUNT_P(return_value, 1);
	Z_SET_ISREF_P(return_value);

	s = zend_object_store_get_object(return_value TSRMLS_CC);
	s->s = search;

	s->refhandle = *getThis();
	zend_objects_store_add_ref(getThis() TSRMLS_CC);
	s->h = h;
}
/* }}} */

/* {{{ proto object Swish::query(string query)
   Executes a search, returning the result object */   
static PHP_METHOD(Swish, query)
{
	char *query = NULL;
	int qlen;
	struct php_sw_handle *h;
	struct php_sw_results *r;
	SW_RESULTS results;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &query, &qlen)) {
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

	object_init_ex(return_value, ce_sw_results);
	Z_SET_REFCOUNT_P(return_value, 1);
	Z_SET_ISREF_P(return_value);
	r = zend_object_store_get_object(return_value TSRMLS_CC);
	r->r = results;
	r->h = h;

	r->refhandle = *getThis();
	zend_objects_store_add_ref(&r->refhandle TSRMLS_CC);
}
/* }}} */

/* {{{ proto array Swish::getMetaList(string indexname)
   Returns an array of meta information for a named index */
static PHP_METHOD(Swish, getMetaList)
{
	struct php_sw_handle *h;
	char *index_name;
	int len;
	SWISH_META_LIST meta;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &index_name, &len)) {
		return;
	}

	h = (struct php_sw_handle*)zend_object_store_get_object(getThis() TSRMLS_CC);

	meta = SwishMetaList(h->h, index_name);
	fill_property_list(return_value, meta TSRMLS_CC);
}
/* }}} */

/* {{{ proto array Swish::getPropertyList(string indexname)
   Returns an array of property information for a named index */
static PHP_METHOD(Swish, getPropertyList)
{
	struct php_sw_handle *h;
	char *index_name;
	int len;
	SWISH_META_LIST meta;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &index_name, &len)) {
		return;
	}

	h = (struct php_sw_handle*)zend_object_store_get_object(getThis() TSRMLS_CC);

	meta = SwishPropertyList(h->h, index_name);
	fill_property_list(return_value, meta TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SwishSearch::setStructure(int structure)
   Sets the "structure" flag in the search object */
static PHP_METHOD(SwishSearch, setStructure)
{
	struct php_sw_search *s;
	long l;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &l)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishSetStructure(s->s, l);
}
/* }}} */

/* {{{ proto void SwishSearch::setPhraseDelimiter(string delim)
   Sets the phrase delimiter character, the default is double-quotes */
static PHP_METHOD(SwishSearch, setPhraseDelimiter)
{
	struct php_sw_search *s;
	char *str;
	int str_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len)) {
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
	int str_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);

	SwishSetSort(s->s, str);
}
/* }}} */

/* {{{ proto void SwishSearch::setLimit(string prop, string low, string hi)
   Sets the limit parameters for a search */
static PHP_METHOD(SwishSearch, setLimit)
{
	struct php_sw_search *s;
	char *prop, *low, *hi;
	int pl, ll, hl;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &prop, &pl, &low, &ll, &hi, &hl)) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (SwishSetSearchLimit(s->s, prop, low, hi) == 0) {
		if (sw_throw_exception(s->h TSRMLS_CC)) {
			return;
		}
	}
}
/* }}} */

/* {{{ proto void SwishSearch::resetLimit()
   Resets the limit parameters for a search */
static PHP_METHOD(SwishSearch, resetLimit)
{
	struct php_sw_search *s;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	s = (struct php_sw_search*)zend_object_store_get_object(getThis() TSRMLS_CC);
	SwishResetSearchLimit(s->s);
}
/* }}} */

/* {{{ proto object SwishSearch::execute([string query])
   Returns a new results object */
static PHP_METHOD(SwishSearch, execute)
{
	char *query = NULL;
	int qlen;
	struct php_sw_search *s;
	struct php_sw_results *r;
	SW_RESULTS results;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &query, &qlen)) {
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

	object_init_ex(return_value, ce_sw_results);
	Z_SET_REFCOUNT_P(return_value, 1);
	Z_SET_ISREF_P(return_value);
	r = zend_object_store_get_object(return_value TSRMLS_CC);
	r->r = results;
	r->h = s->h;

	r->refhandle = s->refhandle;
	zend_objects_store_add_ref(&s->refhandle TSRMLS_CC);
}
/* }}} */

/* {{{ proto long SwishResults::seekResult(int position)
   Sets the current seek position in the list of results. 0 is the first. */
static PHP_METHOD(SwishResults, seekResult)
{
	struct php_sw_results *r;
	long l;
	int retval;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &l)) {
		return;
	}

	if (l < 0) {
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, "position cannot be less than zero");
		return;
	}

	r = (struct php_sw_results*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (!r->r) {
		zend_throw_exception_ex(ce_sw_exception, 0 TSRMLS_CC, "no more results");
		return;
	}

	retval = SwishSeekResult(r->r, l);
	if (sw_throw_exception(r->h TSRMLS_CC)) {
		return;
	}
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto object SwishResults::nextResult()
   Advances to the next result. Returns false when there are no more results. */
static PHP_METHOD(SwishResults, nextResult)
{
	struct php_sw_results *r;
	struct php_sw_result *result;
	SW_RESULT res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
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

	object_init_ex(return_value, ce_sw_result);
	Z_SET_REFCOUNT_P(return_value, 1);
	Z_SET_ISREF_P(return_value);
	result = zend_object_store_get_object(return_value TSRMLS_CC);
	result->r = res;
	result->h = r->h;

	result->refhandle = r->refhandle;
	zend_objects_store_add_ref(&r->refhandle TSRMLS_CC);
}
/* }}} */

/* {{{ proto array SwishResults::getParsedWords(string indexname)
   Returns the tokenized query */
static PHP_METHOD(SwishResults, getParsedWords)
{
	struct php_sw_results *r;
	SWISH_HEADER_VALUE value;
	char *index;
	int index_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_len)) {
		return;
	}

	r = (struct php_sw_results*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (!r->r) {
		RETURN_FALSE;
	}

	value = SwishParsedWords(r->r, index);
	if (!value.string_list) {
		RETURN_FALSE;
	}

	php_sw_header_to_zval(value, SWISH_LIST, &return_value, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto array SwishResults::getRemovedStopwords(string indexname)
   Returns a list of stopwords removed from the query */
static PHP_METHOD(SwishResults, getRemovedStopwords)
{
	struct php_sw_results *r;
	SWISH_HEADER_VALUE value;
	char *index;
	int index_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_len)) {
		return;
	}

	r = (struct php_sw_results*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (!r->r) {
		RETURN_FALSE;
	}

	value = SwishRemovedStopwords(r->r, index);
	if (!value.string_list) {
		RETURN_FALSE;
	}

	php_sw_header_to_zval(value, SWISH_LIST, &return_value, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto array SwishResult::getMetaList()
   Returns an array of meta information for the result */
static PHP_METHOD(SwishResult, getMetaList)
{
	struct php_sw_result *r;
	SWISH_META_LIST meta;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	r = (struct php_sw_result*)zend_object_store_get_object(getThis() TSRMLS_CC);

	meta = SwishResultMetaList(r->r);
	fill_property_list(return_value, meta TSRMLS_CC);
}
/* }}} */

/* {{{ proto array SwishResult::stem(string word)
   Stems the word and returns result as an array */
static PHP_METHOD(SwishResult, stem)
{
	struct php_sw_result *r;
	SW_FUZZYWORD fuzzy_word;
	const char** word_list;
	char *word;
	int word_len;
	zval *tmp;
	int error;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &word, &word_len)) {
		return;
	}

	r = (struct php_sw_result*)zend_object_store_get_object(getThis() TSRMLS_CC);

	fuzzy_word = SwishFuzzyWord(r->r, word);
	if (sw_throw_exception(r->h TSRMLS_CC)) {
		return;
	}

	if (!fuzzy_word) {
		RETURN_FALSE;
	}

	word_list = SwishFuzzyWordList(fuzzy_word);
	error = SwishFuzzyWordError(fuzzy_word);
	if (error) {
		sw_stemmer_error_to_exception(error TSRMLS_CC);
		SwishFuzzyWordFree(fuzzy_word);
		RETURN_FALSE;
	}

	array_init(return_value);
	while (word_list && *word_list) {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, (char *)(*word_list), 1);
		add_next_index_zval(return_value, tmp);

		word_list++;
	}
	SwishFuzzyWordFree(fuzzy_word);
}
/* }}} */

/* {{{ class method tables */
static zend_function_entry sw_handle_methods[] = {
	PHP_ME(Swish, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Swish, prepare, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Swish, query, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Swish, getMetaList, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Swish, getPropertyList, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry sw_search_methods[] = {
	PHP_ME(SwishSearch, execute, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setStructure, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setPhraseDelimiter, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setSort, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, setLimit, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishSearch, resetLimit, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry sw_results_methods[] = {
	PHP_ME(SwishResults, seekResult, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishResults, nextResult, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishResults, getParsedWords, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishResults, getRemovedStopwords, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry sw_result_methods[] = {
	PHP_ME(SwishResult, getMetaList, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SwishResult, stem, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ swish_functions[] */
static zend_function_entry swish_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

#ifdef COMPILE_DL_SWISH
ZEND_GET_MODULE(swish)
#endif

#define PHP_SWISH_CLASS_CONST(name, value) \
		zend_declare_class_constant_long(ce_sw_handle, name, sizeof(name)-1, (long)value TSRMLS_CC);

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(swish)
{
	zend_class_entry ce;

	memcpy(&cannot_be_cloned, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	cannot_be_cloned.clone_obj = NULL;

	memcpy(&php_sw_result_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sw_result_obj_handlers.clone_obj = NULL;
	php_sw_result_obj_handlers.read_property = php_sw_result_read_property;
	php_sw_result_obj_handlers.get_properties = php_sw_result_get_properties;

	memcpy(&php_sw_handle_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sw_handle_obj_handlers.clone_obj = NULL;
	php_sw_handle_obj_handlers.read_property = php_sw_handle_read_property;
	php_sw_handle_obj_handlers.get_properties = php_sw_handle_get_properties;

	memcpy(&php_sw_results_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sw_results_obj_handlers.clone_obj = NULL;
	php_sw_results_obj_handlers.read_property = php_sw_results_read_property;
	php_sw_results_obj_handlers.get_properties = php_sw_results_get_properties;


	INIT_CLASS_ENTRY(ce, "SwishException", NULL);
	ce_sw_exception = zend_register_internal_class_ex(&ce, 
			zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "Swish", sw_handle_methods);
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

	PHP_SWISH_CLASS_CONST("META_TYPE_UNDEF",  SW_META_TYPE_UNDEF);
	PHP_SWISH_CLASS_CONST("META_TYPE_STRING", SW_META_TYPE_STRING);
	PHP_SWISH_CLASS_CONST("META_TYPE_ULONG",  SW_META_TYPE_ULONG);
	PHP_SWISH_CLASS_CONST("META_TYPE_DATE",	  SW_META_TYPE_DATE);

	PHP_SWISH_CLASS_CONST("IN_FILE_BIT",       IN_FILE_BIT);
	PHP_SWISH_CLASS_CONST("IN_TITLE_BIT",      IN_TITLE_BIT);
	PHP_SWISH_CLASS_CONST("IN_HEAD_BIT",       IN_HEAD_BIT);
	PHP_SWISH_CLASS_CONST("IN_BODY_BIT",       IN_BODY_BIT);
	PHP_SWISH_CLASS_CONST("IN_COMMENTS_BIT",   IN_COMMENTS_BIT);
	PHP_SWISH_CLASS_CONST("IN_HEADER_BIT",     IN_HEADER_BIT);
	PHP_SWISH_CLASS_CONST("IN_EMPHASIZED_BIT", IN_EMPHASIZED_BIT);
	PHP_SWISH_CLASS_CONST("IN_META_BIT",       IN_META_BIT);

	PHP_SWISH_CLASS_CONST("IN_FILE",       IN_FILE);
	PHP_SWISH_CLASS_CONST("IN_TITLE",      IN_TITLE);
	PHP_SWISH_CLASS_CONST("IN_HEAD",       IN_HEAD);
	PHP_SWISH_CLASS_CONST("IN_BODY",       IN_BODY);
	PHP_SWISH_CLASS_CONST("IN_COMMENTS",   IN_COMMENTS);
	PHP_SWISH_CLASS_CONST("IN_HEADER",     IN_HEADER);
	PHP_SWISH_CLASS_CONST("IN_EMPHASIZED", IN_EMPHASIZED);
	PHP_SWISH_CLASS_CONST("IN_META",       IN_META);
	PHP_SWISH_CLASS_CONST("IN_ALL",        IN_ALL);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(swish)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "swish support", "enabled");
	php_info_print_table_row(2, "extension version", PHP_SWISH_VERSION);
	/* DO NOT REMOVE THIS URL!
	 * It is here for license compliance */
	php_info_print_table_row(2, "source available from", "http://swish-e.org");
	php_info_print_table_end();
}
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
	PHP_SWISH_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
