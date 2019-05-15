#ifndef _DATE_H_INCLUDED_
#define _DATE_H_INCLUDED_

typedef struct date Date;
struct date {
    void *self;            /* instance-specific data */

/*
 * duplicate() creates a duplicate of `d'
 * returns pointer to new Date structure if successful,
 *         NULL if not (memory allocation failure)
 */
    const Date *(*duplicate)(const Date *d);

/*
 * compare() compares two dates, returning <0, 0, >0 if
 * date1<date2, date1==date2, date1>date2, respectively
 */
    int (*compare)(const Date *date1, const Date *date2);

/*
 * destroy() returns any storage associated with `d' to the system
 */
    void (*destroy)(const Date *d);
};

/*
 * Date_create() creates a Date structure from `datestr`
 * `datestr' is expected to be of the form "dd/mm/yyyy"
 * returns pointer to Date structure if successful,
 *         NULL if not (syntax error)
 */
const Date *Date_create(char *datestr);

#endif /* _DATE_H_INCLUDED_ */
