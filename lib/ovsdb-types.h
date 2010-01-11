/* Copyright (c) 2009, 2010 Nicira Networks
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OVSDB_TYPES_H
#define OVSDB_TYPES_H 1

#include <stdbool.h>
#include <stdint.h>
#include "compiler.h"
#include "uuid.h"

struct json;

/* An atomic type: one that OVSDB regards as a single unit of data. */
enum ovsdb_atomic_type {
    OVSDB_TYPE_VOID,            /* No value. */
    OVSDB_TYPE_INTEGER,         /* Signed 64-bit integer. */
    OVSDB_TYPE_REAL,            /* IEEE 754 double-precision floating point. */
    OVSDB_TYPE_BOOLEAN,         /* True or false. */
    OVSDB_TYPE_STRING,          /* UTF-8 string. */
    OVSDB_TYPE_UUID,            /* RFC 4122 UUID referencing a table row. */
    OVSDB_N_TYPES
};

static inline bool ovsdb_atomic_type_is_valid(enum ovsdb_atomic_type);
static inline bool ovsdb_atomic_type_is_valid_key(enum ovsdb_atomic_type);
bool ovsdb_atomic_type_from_string(const char *, enum ovsdb_atomic_type *);
struct ovsdb_error *ovsdb_atomic_type_from_json(enum ovsdb_atomic_type *,
                                                const struct json *);
const char *ovsdb_atomic_type_to_string(enum ovsdb_atomic_type);
struct json *ovsdb_atomic_type_to_json(enum ovsdb_atomic_type);

/* An OVSDB type.
 *
 * Several rules constrain the valid types.  See ovsdb_type_is_valid() (in
 * ovsdb-types.c) for details.
 *
 * If 'value_type' is OVSDB_TYPE_VOID, 'n_min' is 1, and 'n_max' is 1, then the
 * type is a single atomic 'key_type'.
 *
 * If 'value_type' is OVSDB_TYPE_VOID and 'n_min' or 'n_max' (or both) has a
 * value other than 1, then the type is a set of 'key_type'.  If 'n_min' is 0
 * and 'n_max' is 1, then the type can also be considered an optional
 * 'key_type'.
 *
 * If 'value_type' is not OVSDB_TYPE_VOID, then the type is a map from
 * 'key_type' to 'value_type'.  If 'n_min' is 0 and 'n_max' is 1, then the type
 * can also be considered an optional pair of 'key_type' and 'value_type'.
 */
struct ovsdb_type {
    enum ovsdb_atomic_type key_type;
    enum ovsdb_atomic_type value_type;
    unsigned int n_min;
    unsigned int n_max;         /* UINT_MAX stands in for "unlimited". */
};

#define OVSDB_TYPE_SCALAR_INITIALIZER(KEY_TYPE) \
        { KEY_TYPE, OVSDB_TYPE_VOID, 1, 1 }

extern const struct ovsdb_type ovsdb_type_integer;
extern const struct ovsdb_type ovsdb_type_real;
extern const struct ovsdb_type ovsdb_type_boolean;
extern const struct ovsdb_type ovsdb_type_string;
extern const struct ovsdb_type ovsdb_type_uuid;

bool ovsdb_type_is_valid(const struct ovsdb_type *);

static inline bool ovsdb_type_is_scalar(const struct ovsdb_type *);
static inline bool ovsdb_type_is_optional(const struct ovsdb_type *);
static inline bool ovsdb_type_is_composite(const struct ovsdb_type *);
static inline bool ovsdb_type_is_set(const struct ovsdb_type *);
static inline bool ovsdb_type_is_map(const struct ovsdb_type *);

char *ovsdb_type_to_english(const struct ovsdb_type *);

struct ovsdb_error *ovsdb_type_from_json(struct ovsdb_type *,
                                         const struct json *)
    WARN_UNUSED_RESULT;
struct json *ovsdb_type_to_json(const struct ovsdb_type *);

/* Inline function implementations. */

static inline bool
ovsdb_atomic_type_is_valid(enum ovsdb_atomic_type atomic_type)
{
    return atomic_type >= 0 && atomic_type < OVSDB_N_TYPES;
}

static inline bool
ovsdb_atomic_type_is_valid_key(enum ovsdb_atomic_type atomic_type)
{
    /* XXX should we disallow reals or booleans as keys? */
    return ovsdb_atomic_type_is_valid(atomic_type);
}

static inline bool ovsdb_type_is_scalar(const struct ovsdb_type *type)
{
    return (type->value_type == OVSDB_TYPE_VOID
            && type->n_min == 1 && type->n_max == 1);
}

static inline bool ovsdb_type_is_optional(const struct ovsdb_type *type)
{
    return type->n_min == 0;
}

static inline bool ovsdb_type_is_composite(const struct ovsdb_type *type)
{
    return type->n_max > 1;
}

static inline bool ovsdb_type_is_set(const struct ovsdb_type *type)
{
    return (type->value_type == OVSDB_TYPE_VOID
            && (type->n_min != 1 || type->n_max != 1));
}

static inline bool ovsdb_type_is_map(const struct ovsdb_type *type)
{
    return type->value_type != OVSDB_TYPE_VOID;
}

#endif /* ovsdb-types.h */
