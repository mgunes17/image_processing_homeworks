#include <stdio.h>
#include <stdlib.h>
#include "imageProperty.h"
#include "structTypes.h"
#include "histogram.h"

int createNewImage(IMAGE , HISTOGRAM );

int main() {
	IMAGE image;
	HEADER header;
	HISTOGRAM histogram;

	printf("fotgrafin konumunu giriniz:");
	scanf("%s", image.path);

	header = readProperties(image.path);
	image.header = header;
	image.totalSize = computeTotalSize(image);
	image.headerSize = computeHeaderSize(image);

	//test amacli
	/*printf("%s %s %d %d %d",
			image.header.title,
			image.header.comment,
			image.header.width,
			image.header.height,
			image.header.maxGrayValue);*/

	histogram = histogramEqualization(image);

	int control = createNewImage(image, histogram);

	if(control == 0)
		printf("yeni goruntu olusturuldu");
	else
		printf("yeni goruntu olusturulamadi");

	return 0;
}


int createNewImage(IMAGE image, HISTOGRAM histogram) {
	FILE *newImage;
	newImage = fopen("newImage.pgm", "w+b");
	int i;
	unsigned char oldPx, newPx;

	if(newImage == NULL) {
		printf("yeni resim olusturulamadi");
		return 1;
	}

	FILE *file;
	file = fopen(image.path, "rb");

	if(file == NULL) {
		printf("Resim acilamadi");
		exit(1);
	}

	//header i kopyala
	for(i=0; i<image.headerSize; i++) {
		fread(&oldPx, sizeof(unsigned char), 1, file);
		fwrite(&oldPx, sizeof(unsigned char), 1, newImage);
	}

	//gri seviyelerine karsilik gelen yeni degerleri bul
	for(i=image.headerSize; i<image.totalSize; i++) {
		fread(&oldPx, sizeof(unsigned char), 1, file);
		newPx = (unsigned char)histogram.newValueOfPx[oldPx];
		fwrite(&newPx, sizeof(unsigned char), 1, newImage);
	}

	fclose(newImage);
	fclose(file);

	return 0;
}

