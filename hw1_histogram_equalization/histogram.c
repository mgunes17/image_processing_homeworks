/*
 * histogram.c
 *
 *  Created on: Oct 13, 2016
 *      Author: mustafa
 */

#include <stdio.h>
#include <stdlib.h>
#include "structTypes.h"
#include "histogram.h"

HISTOGRAM histogramEqualization(IMAGE image) {
	FILE *file;
	file = fopen(image.path, "rb");
	int i;

	if(file == NULL) {
		printf("Resim acilamadi");
		exit(1);
	}

	unsigned char n = 0;
	HISTOGRAM histogram;

	//maxGray kadar dinamik yer ac
	histogram.numberOfPx = (int *) calloc (image.header.maxGrayValue + 1, sizeof(int));
	histogram.rateOfPx = (double *) calloc (image.header.maxGrayValue + 1, sizeof(double));
	histogram.cdfValueOfPx = (double *) calloc (image.header.maxGrayValue + 1, sizeof(double));
	histogram.newValueOfPx = (int *) calloc (image.header.maxGrayValue + 1, sizeof(int));

	if(!histogram.numberOfPx || !histogram.rateOfPx || !histogram.cdfValueOfPx || !histogram.newValueOfPx) {
		printf("yer ayrilamadi");
		exit(1);
	}

	//her gri seviyesinin kaç tane oldugunu bul
	for(i=image.headerSize; i<image.totalSize; i++) {
		fread(&n, sizeof(unsigned char), 1, file);
		histogram.numberOfPx[n]++;
	}

	fclose(file);

	//histogrami olustur
	computeHistogramValues(image, &histogram);

	return histogram;
}

void computeHistogramValues(IMAGE image, HISTOGRAM * histogram) {
	double fraction = image.header.height * image.header.width;
	int i;

	for(int i=0; i<image.header.maxGrayValue+1; i++) {
		histogram->rateOfPx[i] = histogram->numberOfPx[i] / fraction;
	}

	//distributedCumulative distribution function değerlerini hesapla
	histogram->cdfValueOfPx[0] = histogram->rateOfPx[0];

	for(i=1; i<image.header.maxGrayValue+1; i++) {
		histogram->cdfValueOfPx[i] = histogram->rateOfPx[i] + histogram->cdfValueOfPx[i-1];
	}

	//new values of px i oluştur
	for(i=0; i<image.header.maxGrayValue+1; i++) {
		histogram->newValueOfPx[i] = round(histogram->cdfValueOfPx[i], image.header.maxGrayValue);
	}

}


int round(double rounded, double maxValue) {
	double i = 0;
	int newValue;

	while(i <= maxValue && rounded > (i/maxValue)) {
		i++;
	}

	if(i == maxValue)
		return newValue= maxValue;

	else if((rounded-(i-1)/maxValue) > i/maxValue-rounded)
		return newValue= maxValue = i;
	else
		return newValue = i-1;
}
