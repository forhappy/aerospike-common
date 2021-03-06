/* 
 * Copyright 2008-2014 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#pragma once

#include <aerospike/as_integer.h>
#include <aerospike/as_bytes.h>
#include <aerospike/as_list.h>
#include <aerospike/as_map.h>
#include <aerospike/as_string.h>
#include <aerospike/as_util.h>
#include <aerospike/as_val.h>

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
 *	TYPES
 *****************************************************************************/

struct as_rec_hooks_s;

/**
 *	Callback function for `as_rec_foreach()`. Called for each bin in the 
 *	record.
 *	
 *	@param name 	The name of the current bin.
 *	@param value 	The value of the current bin.
 *	@param udata	The user-data provided to the `as_rec_foreach()`.
 *	
 *	@return true to continue iterating through the list. 
 *			false to stop iterating.
 */
typedef bool (* as_rec_foreach_callback) (const char * name, const as_val * value, void * udata);

/**
 *	as_rec is an interface for record types. A record is how data in Aerospike
 *	is represented, and is composed of bins and metadata.
 *
 *	Implementations:
 *	- as_record
 *	
 *	@extends as_val
 *	@ingroup aerospike_t
 */
typedef struct as_rec_s {

	/**
	 *	@private
	 *	as_boolean is a subtype of as_val.
	 *	You can cast as_boolean to as_val.
	 */
	as_val _;

	/**
	 *	Data provided by the implementation of `as_rec`.
	 */
	void * data;

	/**
	 *	Hooks provided by the implementation of `as_rec`.
	 */
	const struct as_rec_hooks_s * hooks;

} as_rec;

/**
 *	Record Hooks.
 *
 *	An implementation of `as_rec` should provide implementations for each
 *	of the hooks.
 */
typedef struct as_rec_hooks_s {

	/**
	 *	Destroy the record.
	 */
	bool (* destroy)(as_rec * rec);

	/**
	 *	Get the hashcode of the record.
	 */
	uint32_t (* hashcode)(const as_rec * rec);

	/**
	 *	Get the value of the bin in the record.
	 */
	as_val * (* get)(const as_rec * rec, const char * name);

	/**
	 *	Set the value of the bin in the record.
	 */
	int (* set)(const as_rec * rec, const char * name, const as_val * value);

	/**
	 *	Remove the bin from the record.	
	 */
	int (* remove)(const as_rec * rec, const char * bin);

	/**
	 *	Get the ttl value of the record.
	 */
	uint32_t (* ttl)(const as_rec  * rec);

	/**
	 *	Get the generation value of the record.
	 */
	uint16_t (* gen)(const as_rec * rec);

	/**
	 *	Get the number of bins of the record.
	 */
	uint16_t (* numbins)(const as_rec * rec);

	/**
	 *	Get the digest of the record.
	 */
	as_bytes * (* digest)(const as_rec * rec);

	/**
	 *	Set flags on a bin.
	 */
	int (* set_flags)(const as_rec * rec, const char * bin, uint8_t flags);

	/**
	 *	Set the type of record.
	 */
	int (* set_type)(const as_rec * rec,  uint8_t type);

	/**
	 *	Iterate over each bin in the record.
	 */
	bool (* foreach)(const as_rec * rec, as_rec_foreach_callback callback, void * udata);

} as_rec_hooks;

/******************************************************************************
 *	INSTANCE FUNCTIONS
 *****************************************************************************/

/**
 *	@private
 *	Utilized by subtypes of as_rec to initialize the parent.
 *
 *	@param rec		The record to initialize
 *	@param free 	If TRUE, then as_rec_destory() will free the record.
 *	@param data		Data for the map.
 *	@param hooks	Implementation for the map interface.
 *	
 *	@return The initialized as_map on success. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
as_rec * as_rec_cons(as_rec * rec, bool free, void * data, const as_rec_hooks * hooks);

/**
 *	Initialize a stack allocated record.
 *
 *	@param rec		Stack allocated record to initialize.
 *	@param data		Data for the record.
 *	@param hooks	Implementation for the record interface.
 *	
 *	@return On success, the initialized record. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
as_rec * as_rec_init(as_rec * rec, void * data, const as_rec_hooks * hooks);

/**
 *	Create and initialize a new heap allocated record.
 *	
 *	@param data		Data for the record.
 *	@param hooks	Implementation for the record interface.
 *	
 *	@return On success, a new record. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
as_rec * as_rec_new(void * data, const as_rec_hooks * hooks);

/**
 *	Destroy the record.
 *
 *	@relatesalso as_rec
 */
static inline void as_rec_destroy(as_rec * rec) 
{
	as_val_destroy((as_val *) rec);
}

/******************************************************************************
 *	INLINE FUNCTIONS
 ******************************************************************************/

/**
 *	Get the data source for the record.
 *
 *	@relatesalso as_rec
 */
static inline void * as_rec_source(const as_rec * rec) 
{
	return rec ? rec->data : NULL;
}

/**
 *	Remove a bin from a record.
 *
 *	@param rec		The record to remove the bin from.
 *	@param name 	The name of the bin to remove.
 *
 *	@return 0 on success, otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_remove(const as_rec * rec, const char * name) 
{
	return as_util_hook(remove, 1, rec, name);
}

/**
 *	Get the ttl for the record.
 *
 *	@relatesalso as_rec
 */
static inline uint32_t as_rec_ttl(const as_rec * rec) 
{
	return as_util_hook(ttl, 0, rec);
}

/**
 *	Get the generation of the record
 *
 *	@relatesalso as_rec
 */
static inline uint16_t as_rec_gen(const as_rec * rec) 
{
	return as_util_hook(gen, 0, rec);
}

/**
 *	Get the number of bins in the record.
 *
 *	@relatesalso as_rec
 */
static inline uint16_t as_rec_numbins(const as_rec * rec) 
{
	return as_util_hook(numbins, 0, rec);
}

/**
 *	Get the digest of the record.
 *
 *	@relatesalso as_rec
 */
static inline as_bytes * as_rec_digest(const as_rec * rec) 
{
	return as_util_hook(digest, 0, rec);
}

/**
 *	Set flags on a bin.
 *
 *	@relatesalso as_rec
 */
static inline int  as_rec_set_flags(const as_rec * rec, const char * name, uint8_t flags) 
{
	return as_util_hook(set_flags, 0, rec, name, flags);
}

/**
 *	Set the record type.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_type(const as_rec * rec, uint8_t rec_type) 
{
	return as_util_hook(set_type, 0, rec, rec_type);
}

/******************************************************************************
 *	BIN GETTER FUNCTIONS
 ******************************************************************************/

/**
 *	Get a bin's value.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline as_val * as_rec_get(const as_rec * rec, const char * name) 
{
	return as_util_hook(get, NULL, rec, name);
}

/**
 *	Get a bin's value as an int64_t.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise 0.
 *
 *	@relatesalso as_rec
 */
static inline int64_t as_rec_get_int64(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	as_integer * i = as_integer_fromval(v);
	return i ? as_integer_toint(i) : 0;
}

/**
 *	Get a bin's value as a NULL terminated string.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline char * as_rec_get_str(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	as_string * s = as_string_fromval(v);
	return s ? as_string_tostring(s) : 0;
}

/**
 *	Get a bin's value as an as_integer.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline as_integer * as_rec_get_integer(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	return as_integer_fromval(v);
}

/**
 *	Get a bin's value as an as_string.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name		The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline as_string * as_rec_get_string(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	return as_string_fromval(v);
}

/**
 *	Get a bin's value as an as_bytes.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline as_bytes * as_rec_get_bytes(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	return as_bytes_fromval(v);
}

/**
 *	Get a bin's value as an as_list.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline as_list * as_rec_get_list(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	return as_list_fromval(v);
}

/**
 *	Get a bin's value as an as_map.
 *
 *	@param rec		The as_rec to read the bin value from.
 *	@param name 	The name of the bin.
 *
 *	@return On success, the value of the bin. Otherwise NULL.
 *
 *	@relatesalso as_rec
 */
static inline as_map * as_rec_get_map(const as_rec * rec, const char * name) 
{
	as_val * v = as_util_hook(get, NULL, rec, name);
	return as_map_fromval(v);
}

/******************************************************************************
 *	BIN SETTER FUNCTIONS
 ******************************************************************************/

/**
 *	Set the bin's value to an as_val.
 *
 *	@param rec 		The as_rec to write the bin value to - CONSUMES REFERENCE
 *	@param name 	The name of the bin.
 *	@param value 	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set(const as_rec * rec, const char * name, const as_val * value) 
{
	return as_util_hook(set, 1, rec, name, value);
}

/**
 *	Set the bin's value to an int64_t.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_int64(const as_rec * rec, const char * name, int64_t value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) as_integer_new(value));
}

/**
 *	Set the bin's value to a NULL terminated string.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_str(const as_rec * rec, const char * name, const char * value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) as_string_new_strdup(value));
}

/**
 *	Set the bin's value to an as_integer.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_integer(const as_rec * rec, const char * name, const as_integer * value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) value);
}

/**
 *	Set the bin's value to an as_string.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_string(const as_rec * rec, const char * name, const as_string * value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) value);
}

/**
 *	Set the bin's value to an as_bytes.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_bytes(const as_rec * rec, const char * name, const as_bytes * value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) value);
}

/**
 *	Set the bin's value to an as_list.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_list(const as_rec * rec, const char * name, const as_list * value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) value);
}

/**
 *	Set the bin's value to an as_map.
 *
 *	@param rec		The as_rec storing the bin.
 *	@param name 	The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return On success, 0. Otherwise an error occurred.
 *
 *	@relatesalso as_rec
 */
static inline int as_rec_set_map(const as_rec * rec, const char * name, const as_map * value) 
{
	return as_util_hook(set, 1, rec, name, (as_val *) value);
}

/******************************************************************************
 *	ITERATION FUNCTIONS
 ******************************************************************************/

/**
 *	Call the callback function for each bin in the record.
 *
 *	@param rec		The as_rec containing the bins to iterate over.
 *	@param callback	The function to call for each entry.
 *	@param udata	User-data to be passed to the callback.
 *	
 *	@return true if iteration completes fully. false if iteration was aborted.
 *
 *	@relatesalso as_rec
 */
static inline bool as_rec_foreach(const as_rec * rec, as_rec_foreach_callback callback, void * udata) 
{
	return as_util_hook(foreach, false, rec, callback, udata);
}

/******************************************************************************
 *	CONVERSION FUNCTIONS
 ******************************************************************************/

/**
 *	Convert to an as_val.
 *
 *	@relatesalso as_rec
 */
static inline as_val * as_rec_toval(const as_rec * rec) 
{
	return (as_val *) rec;
}

/**
 *	Convert from an as_val.
 *
 *	@relatesalso as_rec
 */
static inline as_rec * as_rec_fromval(const as_val * v) 
{
	return as_util_fromval(v, AS_REC, as_rec);
}

/******************************************************************************
 *	as_val FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Internal helper function for destroying an as_val.
 */
void as_rec_val_destroy(as_val *);

/**
 *	@private
 *	Internal helper function for getting the hashcode of an as_val.
 */
uint32_t as_rec_val_hashcode(const as_val *v);

/**
 *	@private
 *	Internal helper function for getting the string representation of an as_val.
 */
char * as_rec_val_tostring(const as_val *v);
