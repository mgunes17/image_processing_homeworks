/*
 * DisplayImage.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: mgunes
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

typedef struct {
	int clusterId; //px' in ait olduğu küme
	int row; //px in bulunduğu satır sayısı
	int column; //px in bulunduğu sütun sayısı
	int b; //blue değeri
	int g; //green değeri
	int r; //red değeri
	int label; //connected componenet labeling için kullanılacak
} PIXEL; //pixel özelliklerini tutan struct yapısı

typedef struct {
	PIXEL center;
	int size; //o kümeye ait px sayısı
	int sumBlue;
	int sumGreen;
	int sumRed;
} KCENTER; //kümeleri tutan struct yapısı

int findEuclideanDistance(PIXEL, PIXEL);
void connectedLabeling( int , int);
int isSameColor(PIXEL, PIXEL);
void sameLabelChange(int );

int label = 1; //etiketler 1den başlar 0sa etiketlenmemiş
int differentLabelCount = 0;
int colsCount; //çalışılacak resmşn sütun sayısı
int rowsCount; //çalışılacak resmin satır sayısı
PIXEL **labeling; //etiketleme çalışmasının yapılacağı matris

int main( int argc, char** argv ) {
	srand(time(NULL));
	int k; //merkez sayısı
	int epsilon;
	KCENTER * kcenter; //merkezleri tutmak için
	PIXEL * newCenter; //yeni merkezlerle eskileri kıyaslayabilmek için
	int * majority; //eski ve yeni mezkezler arasındaki farkları tutmak için
	char path[30];

	//printf("Resmin yolunu giriniz:");
	//scanf("%s", path);

	Mat img = imread("papagan2.jpg", 1);
	Mat kmImg = imread("papagan2.jpg", 1);
	Mat labelImg = imread("papagan2.jpg", 1);

	/*Mat img = imread(path, 1);
	Mat kmImg = imread(path, 1);
	Mat newImg2 = imread(path, 1);*/

	colsCount = img.cols;
	rowsCount = img.rows;

	printf("%d %d", img.rows, img.cols);

	printf("\nk değerini giriniz:");
	scanf("%d",&k);
	printf("\nepsilon değerini giriniz:");
	scanf("%d", &epsilon);

	kcenter= (KCENTER *) malloc (k * sizeof(KCENTER));
	newCenter = (PIXEL *) malloc (k * sizeof(PIXEL));
	majority = (int *) malloc (k * sizeof(int));

	labeling = (PIXEL **) malloc (img.rows * sizeof(PIXEL *));
	for(int i = 0; i < img.rows; i++) {
		labeling[i] = (PIXEL *) malloc (img.cols * sizeof(PIXEL));
	}

	if(kcenter == NULL || newCenter == NULL || majority == NULL) {
		printf("yer ayrılamadı");
	}

	//rastgele merkez px seçimi
	for(int i = 0; i < k; i++) {
		int row = rand() % rowsCount;
		int column = rand() % colsCount;

		PIXEL px;
		px.row = row;
		px.column = column;

		Vec3b intensity = img.at<Vec3b>(row, column);
		px.b = intensity.val[0];
		px.g = intensity.val[1];
		px.r = intensity.val[2];

		kcenter[i].center = px;
		kcenter[i].size = 0;
		kcenter[i].sumBlue = 0;
		kcenter[i].sumGreen = 0;
		kcenter[i].sumRed = 0;
	}

	int next = 0;
	int ite = 0;

	do {
		ite++;
		printf("\niterasyon:%d\n", ite);

		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				Vec3b intensity = img.at<Vec3b>(i, j);

				PIXEL px;
				px.b = intensity.val[0];
				px.g = intensity.val[1];
				px.r = intensity.val[2];
				px.row = i;
				px.column = j;

				int minDistance = 99999;
				int centerId = -1;

				//en yakın merkezi bul
				for(int p = 0; p < k; p++) {
					PIXEL center = kcenter[p].center;
					int distance = findEuclideanDistance(px, center);
					if(distance < minDistance) {
						centerId = p;
						minDistance = distance;
					}
				}

				//o kümeye ait bilgileri güncelle
				px.clusterId = centerId;
				kcenter[centerId].size++;
				kcenter[centerId].sumBlue += px.b;
				kcenter[centerId].sumRed += px.r;
				kcenter[centerId].sumGreen += px.g;

				//px deki renk değerlerini, ait olduğu merkezinkilere set et
				kmImg.at<Vec3b>(i, j)[0] = kcenter[centerId].center.b;
				kmImg.at<Vec3b>(i, j)[1] = kcenter[centerId].center.g;
				kmImg.at<Vec3b>(i, j)[2] = kcenter[centerId].center.r;
			}
		}

		int sum = 0;

		//yeni merkezleri hesapla
		for(int i = 0; i < k; i++) {
			newCenter[i].b = kcenter[i].sumBlue / kcenter[i].size;
			newCenter[i].g = kcenter[i].sumGreen / kcenter[i].size;
			newCenter[i].r = kcenter[i].sumRed / kcenter[i].size;

			majority[i] = findEuclideanDistance(newCenter[i], kcenter[i].center);
			sum += majority[i];
		}

		//yeni bir iterasyon yapılacaksa merkez px değerlerini güncelle
		if(epsilon < sum) {
			for(int i = 0; i < k; i++) {
				kcenter[i].center = newCenter[i];
				kcenter[i].size = 0;
				kcenter[i].sumBlue = 0;
				kcenter[i].sumGreen = 0;
				kcenter[i].sumRed = 0;
			}
		} else {
			next = 1;
		}

		printf("sum:%d", sum);
	} while(next == 0);

	printf("\nİterasyon Sayısı: %d", ite);

	//labeling

	//k means ile gruplanan resmin labeling matrisine atılması
	for(int i = 0; i < kmImg.rows; i++) {
		for (int j = 0; j < kmImg.cols; j++) {
			Vec3b intensity = kmImg.at<Vec3b>(i, j);
			PIXEL px;
			px.b = intensity.val[0];
			px.g = intensity.val[1];
			px.r = intensity.val[2];
			px.label = 0;
			labeling[i][j] = px;

			//printf("%d   %d  %d\n", px.b, px.g, px. r);
		}
	}

	//komşu ve aynı renkli px lerin gruplanması
	for(int i = 0; i < kmImg.rows; i++) {
		for (int j = 0; j < kmImg.cols; j++) {
			if(labeling[i][j].label == 0) {
				differentLabelCount++;
				label++;
				labeling[i][j].label = label;
				connectedLabeling(i, j);
			}
		}
	}

	//printf("label %d", label);

	//label sayısı kadar rastgele renk oluştur
	PIXEL *labelColor;
	labelColor = (PIXEL *) malloc (label * sizeof(PIXEL));

	for(int i = 0; i < label; i++) {
		labelColor[i].b = rand() % 255;
		labelColor[i].g = rand() % 255;
		labelColor[i].r = rand() % 255;
	}

	for(int i = 0; i < kmImg.rows; i++) {
		for (int j = 0; j < kmImg.cols; j++) {
			labelImg.at<Vec3b>(i, j)[0] = labelColor[labeling[i][j].label].b;
			labelImg.at<Vec3b>(i, j)[1] = labelColor[labeling[i][j].label].g;
			labelImg.at<Vec3b>(i, j)[2] = labelColor[labeling[i][j].label].r;
		}
	}

	imshow("orjinal", img);
	imshow("kmeans", kmImg);
	imshow("labeling", labelImg);

	printf("\n\nBölüt sayısı: %d", differentLabelCount);

	try {
		vector<int> compression_params;
		compression_params.push_back(IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);
		imwrite("mean.png", kmImg, compression_params);
		imwrite("label.png", labelImg, compression_params);
	} catch (runtime_error& ex) {
		fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
		return 1;
	}
	waitKey(0);

	return 0;
}

//verilen pxden başlayıp komşu px ve aynı renk olanları etiketler
void connectedLabeling( int row, int col) {
	if((col + 1 < colsCount) && isSameColor(labeling[row][col + 1], labeling[row][col]) == 1 &&
			labeling[row][col + 1].label != labeling[row][col].label){
		if(labeling[row][col + 1].label != 0) {
			sameLabelChange(labeling[row][col + 1].label);
		}
		labeling[row][col + 1].label = label;
		connectedLabeling(row, col + 1);
	}

	if((row + 1 < rowsCount) && isSameColor(labeling[row + 1][col], labeling[row][col]) == 1 &&
			labeling[row + 1][col].label != labeling[row][col].label) {
		if(labeling[row + 1][col].label != 0) {
			sameLabelChange(labeling[row + 1][col].label);
		}
		labeling[row + 1][col].label = label;
		connectedLabeling(row + 1, col);
	}
}

//bir label a sakip px lere yeni label atanması
void sameLabelChange(int oldLabel){
	int isFind = 0;
	for(int i = 0; i < rowsCount; i++) {
		for( int j = 0; j < colsCount; j++) {
			if(labeling[i][j].label == oldLabel) {
				labeling[i][j].label = label;
				isFind = 1;
			}
		}
	}

	if(isFind == 1)
		differentLabelCount--;
}

//iki px değerinin aynı renk olup olmadığı kontrol eder
int isSameColor(PIXEL p1, PIXEL p2) {
	if(p1.b == p2.b && p1.g == p2.g && p1.r == p2.r) {
		//puts("cc");
		return 1;
	}
	else
		return 0;
}

//iki px arasındaki öklid uzaklığını döndürür
int findEuclideanDistance(PIXEL px1, PIXEL px2) {
	double sum = 0.0;
	sum = pow((px1.b - px2.b) ,2) + pow((px1.g - px2.g), 2) + pow((px1.r - px2.r), 2);
	return (int) sqrt(sum);
}


