#include "Common.h"

#ifndef _H_PUTTEXT_
#define PUTTEXT_H_

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

void GetStringSize(HDC hDC, const char* str, int* w, int* h);
void putTextZH(Mat &dst, const char* str, Point org, Scalar color, int fontSize,
               const char *fn = "Arial", bool italic = false, bool underline = false);

#endif // PUTTEXT_H_
