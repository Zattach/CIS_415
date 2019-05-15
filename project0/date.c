#include "date.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct d_data {
	long day;
	long month;
	long year;
	char *datestr;
} D_Data;

static const Date *d_duplicate(const Date *d){
	D_Data *dd = (D_Data *)d->self;	
	const Date *t = (Date *)malloc(sizeof(Date));
	t = Date_create(dd->datestr);
	return t;
}

/*
 * compare() compares two dates, returning <0, 0, >0 if
 * date1<date2, date1==date2, date1>date2, respectively
 */
static int d_compare(const Date *date1, const Date *date2){
	D_Data *d1 = (D_Data *)date1->self;
	D_Data *d2 = (D_Data *)date2->self;
	int test = 0;
	test += (d1->year - d2->year) * 1000;
	test += (d1->month - d2->month) * 100;
	test += (d1->day - d2->day);
	return test;
}

static void d_destroy(const Date *d){
	free(d->self);
	free((void *)d);
}

Date template = {
	NULL, d_duplicate, d_compare, d_destroy
};

const Date *Date_create(char *datestr){
	Date *d = (Date *)malloc(sizeof(Date));

	if(d != NULL){
		D_Data *dd = (D_Data *)malloc(3 * sizeof(D_Data *));
		if(dd != NULL){
			long dayTemp;
			long monthTemp;
			long yearTemp;
			sscanf(datestr, "%ld/%ld/%ld", &dayTemp, &monthTemp, &yearTemp);
			dd->day = dayTemp;
			dd->month = monthTemp;
			dd->year = yearTemp;
			dd->datestr = datestr;
			*d = template;
			d->self = dd;
		}else{
			free(d);
			d = NULL;
		}
	}
	return d;
}
