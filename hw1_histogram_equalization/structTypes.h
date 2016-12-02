/*
 * structTypes.h
 *
 *  Created on: Oct 13, 2016
 *      Author: mustafa
 */

#ifndef STRUCTTYPES_H_
#define STRUCTTYPES_H_

//pgm formatının header bilgilerini tutan struct
typedef struct {
	char title[10];
	char comment[100];
	int width;
	int height;
	int maxGrayValue;
} HEADER;

//islem yapilacak resmin ozelliklerini tutan struct
typedef struct {
	char path[30];
	int headerSize;
	int totalSize;
	HEADER header;
} IMAGE;

//resme ait px özelliklerini tutan struct
typedef struct {
	int *numberOfPx;
	double *rateOfPx;
	double *cdfValueOfPx;
	int *newValueOfPx;
} HISTOGRAM;

#endif /* STRUCTTYPES_H_ */
