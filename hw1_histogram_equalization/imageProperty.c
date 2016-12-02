/*
 * imageProperty.c
 *
 *  Created on: Oct 13, 2016
 *      Author: mustafa
 */

#include <stdio.h>
#include <stdlib.h>
#include "imageProperty.h"
#include "structTypes.h"

HEADER readProperties(char path[30]) {
	FILE *file;
	HEADER header;

	file = fopen(path, "rb");

	if(file == NULL) {
		printf("RESIM ACILAMADI");
		exit(1);
	}

	fscanf(file, "%s", header.title);
	fgetc(file); // '\n' karakterini gecmek icin

	while(fgetc(file) == '#') { //yorum var, \n gorene kadar oku
		int i = 0;

		while((header.comment[i] = fgetc(file)) != '\n') {
			i++;
		}
	}

	//yorum yoksa okunan karakteri kaybetmemek icin geri al
	fseek(file, -1, SEEK_CUR);

	fscanf(file, "%d %d %d", &header.width, &header.height, &header.maxGrayValue);
	fclose(file);

	return header;
}

int computeHeaderSize(IMAGE image) {
	return image.totalSize - (image.header.width * image.header.height);
}

int computeTotalSize(IMAGE image) {
	FILE *file;
	long int totalSize;
	file = fopen(image.path, "rb");

	if(file == NULL) {
		printf("Resim acilamadi");
		exit(1);
	}
	else {
		fseek(file, 0, SEEK_END);
		totalSize = ftell(file);
		fseek(file, 0, SEEK_SET);
	}

	fclose(file);
	return totalSize;
}
