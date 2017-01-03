/*
 * main.c
 *
 *  Created on: Dec 28, 2016
 *      Author: mgunes
 */

#include <opencv2/opencv.hpp>
#include <iostream>

#include <stdlib.h>
#include <string.h>

using namespace cv;
using namespace std;

//histogram bilgisini tutan struct yapısı
typedef struct {
	char path[50];
	int histogram[64];
	int similarity; //test e olan yakınlık değeri
} IMAGE;

//prototipler
int euclideanDistance(int arr1[100], int arr2[100], int size);
void createColorHistogram(IMAGE *image);
int computeTransition(int lbp[8]);
int computeSecond(int color, int first);
void createLBPHistogram(IMAGE *image);
void readImageNames(IMAGE *image, char name[50]);
void similarImageForColor(IMAGE test, IMAGE *colorImage);
void similarImageForTexture(IMAGE test, IMAGE textureImage[50]);
void trainColorImages(IMAGE *colorImage);
void trainTextureImages(IMAGE *textureImage);
int valueForColorHistogram(int r, int g, int b);
int valueForLBPHistogram(Mat img, int row, int col);
void sortImages(IMAGE *image);
int transition(int number);

int main() {
	int selection = 0;
	int i, p;

	IMAGE test;
	IMAGE colorImage[50];
	IMAGE textureImage[50];

	char color[50] = "namesColor.txt";
	char texture[50] = "namesTexture.txt";

	//resim isimlerini oku
	readImageNames(colorImage, color);
	readImageNames(textureImage, texture);

	//64 bin histogram için dizileri sıfırlar
	for(i = 0; i < 50; i++) {
		for(p = 0; p < 64; p++) {
			colorImage[i].histogram[p] = 0;
		}
	}

	//lbp için dizileri sıfırla
	for(i = 0; i < 50; i++) {
		for(p = 0; p < 59; p++) {
			textureImage[i].histogram[p] = 0;
		}
	}

	//histogram bilgilerini oluştur
	trainColorImages(colorImage);
	trainTextureImages(textureImage);

	printf("Test resminin adı:");
	scanf("%s", test.path);

	printf("Benzerlik, renk için 1, doku için 2:");
	scanf("%d", &selection);

	if(selection == 1) {
		//test resmi için histogram bilgisi oluştur
		createColorHistogram(&test);

		//test resmine en benzeyenleri bul
		similarImageForColor(test, colorImage);
	} else if(selection == 2) {
		//test resmi için lbp bilgisi oluştur
		createLBPHistogram(&test);

		//test resmine en benzeyenleri bul
		similarImageForTexture(test, textureImage);
	}

	return 0;
}

//renk benzerliği için histogram bilgisini oluştur
void trainColorImages(IMAGE colorImage[50]) {
	int i;

	for(i = 0; i < 50; i++) { //Her bir resim için
		createColorHistogram(&colorImage[i]);
	}
}

//bir resim için histogram bilgisi
void createColorHistogram(IMAGE *image) {
	int p, k, value;
	Mat mat = imread(image->path, 1);

	for(p = 0; p <mat.rows; p++) {
		for(k = 0; k < mat.cols; k++) { // her bir piksel için
			Vec3b intensity = mat.at<Vec3b>(p, k);
			value = valueForColorHistogram(intensity.val[2], intensity.val[1], intensity.val[0]);
			image->histogram[value]++;
		}
	}
}

//test resmine en çok benzeyen resmi bulur
void similarImageForColor(IMAGE test, IMAGE *colorImage) {
	int i;
	int distance;

	for(i = 0; i < 50; i++) {
		distance = euclideanDistance(test.histogram, colorImage[i].histogram, 64);
		colorImage[i].similarity = distance;
	}

	//sırala
	sortImages(colorImage);

	Mat img = imread(colorImage[0].path, 1);
	Mat img2 = imread(colorImage[1].path, 1);
	Mat img3 = imread(colorImage[2].path, 1);
	Mat img4 = imread(colorImage[3].path, 1);
	Mat img5 = imread(colorImage[4].path, 1);
	Mat img6 = imread(colorImage[5].path, 1);

	imshow("1", img);
	imshow("2", img2);
	imshow("3", img3);
	imshow("4", img4);
	imshow("5", img5);
	imshow("6", img6);
	waitKey(0);
}

//uzaklığa göre sırala, en yakın en benzer
void sortImages(IMAGE *image){
	int i, j;

	for(i = 0; i < 49; i++) {
		for(j = i; j < 50; j++) {
			if(image[j].similarity < image[i].similarity) {
				IMAGE temp = image[i];
				image[i] = image[j];
				image[j] = temp;
			}
		}
	}
}

//64 bin histogramı hesaplar
int valueForColorHistogram(int r, int g, int b) {
	int first, second;
	int arr[6];

	first = r >> 7;
	second = computeSecond(r, first);
	arr[5] = first;
	arr[4] = second;

	first = g >> 7;
	second = computeSecond(g, first);
	arr[3] = first;
	arr[2] = second;

	first = b >> 7;
	second = computeSecond(b, first);
	arr[1] = first;
	arr[0] = second;


	return 32 * arr[5] + 16 * arr[4] + 8 * arr[3] + 4 * arr[2] + 2 * arr[1] + 1 * arr[0];
}


//en anlamlı 2.biti hesaplar
int computeSecond(int color, int first) {
	int second;

	if(first == 0) {
		second = (color >> 6);
	} else {
		if((color >> 6) == 1)
			second = 0;
		else
			second = 1;
	}

	return second;
}

//2 histogram dizisi arasındaki öklid uzaklığını hesaplar
int euclideanDistance(int arr1[100], int arr2[100], int size) {
	int distance = 0;
	int i;

	for(i = 0; i < size; i++) {
		distance += pow((arr1[i] - arr2[i]), 2);
	}

	distance = sqrt(distance);
	printf("%d\n", distance);
	return distance;
}

//dosyadan resim isimlerini okur
void readImageNames(IMAGE *image, char name[50]) {
	int i;
	FILE * fp;
	fp = fopen(name, "r");

	if(fp == NULL) {
		printf("Dosya okunamadı.");
		exit(-1);
	}

	for(i = 0; i < 50; i++) {
		fscanf(fp, "%s", image[i].path);
	}

	fclose(fp);
}

void trainTextureImages(IMAGE *textureImage) {
	int i;

	for(i = 0; i < 50; i++) { //Her bir resim için
		createLBPHistogram(&textureImage[i]);
	}
}

void createLBPHistogram(IMAGE *image) {
	int k, p;
	int temp[256];
	Mat mat;
	int t = 0;

	for(k = 0; k < 256; k++) {
		temp[k] = -1;
	}

	mat = imread(image->path, 1);
	cvtColor(mat, mat, COLOR_BGR2GRAY);

	for(p = 1; p <mat.rows - 1; p++) {
		for(k = 1; k < mat.cols - 1; k++) { // her bir piksel için
			int value = valueForLBPHistogram(mat, p, k);

			if(value != -1) {
				temp[value]++;
			} else {
				t++;
			}
		}
	}

	k = 0;
	for(p = 0; p < 256; p++) {
		if(temp[p] != -1) {
			image->histogram[k] = temp[p] + 1;
			k++;
		} else if(transition(p) == 1) {
			image->histogram[k] = 0;
			k++;
		}
	}

	image->histogram[58] = t;
}

//number sayısının 2lik tabandaki karşılığının
//transition olup olmadığını bulur.
int transition(int number) {
	int i;
	int lbp[8];

	for(i = 7; i > -1; i--) {
		lbp[i] = number % 2;
		number = number / 2;
	}

	int transition = computeTransition(lbp);

	if(transition == -1)
		return 0;
	else
		return 1;
}

void similarImageForTexture(IMAGE test, IMAGE textureImage[50]) {
	int i;
	int distance;

	for(i = 0; i < 50; i++) {
		distance = euclideanDistance(test.histogram, textureImage[i].histogram, 59);
		textureImage[i].similarity = distance;
	}

	//sırala
	sortImages(textureImage);

	/////deneme
	for(i = 0; i < 59; i++) {
		printf("%6d %6d\n", test.histogram[i], textureImage->histogram[i]);
	}

	/////deneme

	Mat img = imread(textureImage[0].path, 1);
	Mat img2 = imread(textureImage[1].path, 1);
	Mat img3 = imread(textureImage[2].path, 1);
	Mat img4 = imread(textureImage[3].path, 1);
	Mat img5 = imread(textureImage[4].path, 1);
	Mat img6 = imread(textureImage[5].path, 1);

	imshow("1", img);
	imshow("2", img2);
	imshow("3", img3);
	imshow("4", img4);
	imshow("5", img5);
	imshow("6", img6);
	waitKey(0);

}


//Bir px için lbp dizisini oluşturur
int valueForLBPHistogram(Mat img, int row, int col) {
	int r, c, transition;
	int lbp[8];
	int index = 0;

	Vec3b intensity2 = img.at<Vec3b>(row, col);
	int gray = intensity2.val[0];


	for(r = row - 1; r < row + 2; r++) {
		for(c = col - 1; c < col + 2; c++) {
			if(!(r == row && c == col)) {
				Vec3b intensity1 = img.at<Vec3b>(r, c);
				int neighbour = intensity1.val[0];

				if(neighbour > gray) {
					lbp[index] = 1;
				} else {
					lbp[index] = 0;
				}

				index++;
			}
		}
	}

	transition = computeTransition(lbp);
	return transition;
}

//bir px in lbp değerleri için geçişleri sayar
int computeTransition(int lbp[8]) {
	//geçişleri say
	int transition = 0, index;

	for(index = 0; index < 7; index++) {
		if(lbp[index] != lbp[index + 1])
			transition++;
	}

	if(transition > 2)
		return -1;
	else {
		int value = 0;

		for(index = 0; index < 8; index++) {
			value += pow(2, index) * lbp[index];
		}

		return value;
	}
}
