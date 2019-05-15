/*
 * Copyright (c) 2017, University of Oregon
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Oregon nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "date.h"			/*  so we match the interface signatures  */
#include <stdio.h>			/*  so we can use sscanf()  */
#include <stdlib.h>			/*  so we can use malloc() and free()  */

typedef struct dt_data {	/*  type fpr self  */
	int day;
	int month;
	int year;
} DdData;

static const Date *date_duplicate();
static int date_compare();
static void date_destroy();

/*  helper function to create an instance of DtData from arguments
	returns NULL if malloc failure  */
static cost Date template = {NULL, date_duplicate, date_compare, date_destroy};

static const Date *date_helper(int day, int month, int year){
	Date *d = malloc(sizeof(Date));

	if(d != NULL){
		DtData *dtd = malloc(sizeof(DtData));

		if(dtd != NULL){
			dtd->day = day;
			dtd->month = month;
			dtd->year = year;
			*d = template;
			d->self = dtd;
		}else{
			free(d);
			d = NULL;
		}
	}
	return d;
}

/*  
 *  date_create creates a Date structure from 'datestr'
 *  'datestr' is expected to be of the form "dd/mm/yyyy"
 *  returns pointer to Date structure if successful,
 *  		NULL if not (syntax error)
 */
const Date *Date_create(char *datestr){
	int day;
	int month;
	int year;

	if(sscanf(datestr, "%d/%d/%d/", &day, &month, &year) != 3)
		return NULL;		/*  incorrectly formatted date string  */
	if(day < 1 || day > 31)
		return NULL;		/*  incorrectly formatted date string  */
	if(month < 1 || month > 12)
		return NULL;		/*  incorrectly formatted date string  */
	if(year < 1000 || year > 2018)		/*  current year  */
		return NULL;		/*  incorrectly formatted date string  */
	return date_helper(day, month, year);
}

/*
 *  date_duplicate creates a duplicate of 'd'
 *  returns pointer to a new Date structure if successful,
 *  		NULL if not (memory allocation failure)
 */
static const Date *date_duplicate(const Date *d){
	DtData *dtd = (DtData *)d->self;

	return date_helper(dtd->day, dtd->month, dtd->year);
}

/*
 *  date_compare compares two dates, returning <0, 0, >0 if
 *  date1<date2, date1==date2, date1>date2 respectively
 */
static int date_compare(const Date *date1, const Date *date2){
	DtData *dt1 = (DtData *)date1->self;
	DtData *dt2 = (DtData *)date2->self;

	if(dt1->year != dt2->year)
		return (dt1->year - dt2->year);
	if(dt1->month != dt2->month)
		return (dt1->month - dt2->month);
	return (dt1->day - dt2->day);
}

/*
 *  date_destroy returns any storage associated with 'd' to the system
 */
static void date_destroy(const Date *d){
	free(d->self);
	free((void *)d);
}