//
//  main.cpp
//  O2_xcd
//
//  Created by apple on 5/5/18.
//  Copyright © 2018年 apple. All rights reserved.
//

// O2.cpp : 定义控制台应用程序的入口点。
//
#include "targetver.h"
#include "stdafx.h"
#include "putText.hpp"
#include "main.h"

void main::cvHSV(Mat &input, Mat &mask1, Mat &mask2)
{
    Mat input2;
    input.copyTo(input2);
    Mat inputHSV;//HSV空间
    cvtColor( input, inputHSV, CV_BGR2HSV );
    
    Mat dstTemp1(input.rows, input.cols, CV_8UC1);
    Mat dstTemp2(input.rows, input.cols, CV_8UC1);
    inRange(inputHSV, Scalar(0,43,46), Scalar(10,255,255), dstTemp1);
    inRange(inputHSV, Scalar(160,43,46), Scalar(180,255,255), dstTemp2);
    bitwise_or(dstTemp1, dstTemp2, mask2);
    
    Mat dstTemp3(input2.rows, input2.cols, CV_8UC1);
    Mat dstTemp4(input2.rows, input2.cols, CV_8UC1);
    inRange(inputHSV, Scalar(110,43,46), Scalar(124,255,255), dstTemp3);
    inRange(inputHSV, Scalar(110,43,46), Scalar(124,255,255), dstTemp4);
    bitwise_or(dstTemp3, dstTemp4, mask1);
    //inRange(frameHSV, Scalar(0,30,30), Scalar(180,170,256), dst);
}

void O2::initCQ(int qsize = 10)
{
    Rect newr = Rect(0,0,0,0);
    for (int i = 0; i < qsize; i++)
    {
        q.push(newr);
    }
}

Rect O2::CircularQueue(Rect r)
{
    q.push(r);
    Rect e = q.front();
    q.pop();
    return e;
}

Mat O2::find(Mat &mask1, Mat &mask2, Mat &drawing)
{
    vector<vector<Point>> contours1;
    vector<vector<Point>> filterContours1;
    vector<vector<Point>> contours2;
    vector<vector<Point>> filterContours2;
    vector<Vec4i> hierarchy;
    
    int bheight = drawing.size().height;
    int bwidth = drawing.size().width;
    
    Rect rectangle_blue;
    Rect rectangle_blue_last;
    Rect rectangle_red;
    Rect rectangle_green;
    
    //得到手的轮廓
    findContours(mask1, contours1, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    //去除伪轮廓
    for (size_t i = 0; i < contours1.size(); i++)
    {
        if (fabs(contourArea(Mat(contours1[i]))) > 5000) //判断手进入区域的阈值
        {
            filterContours1.push_back(contours1[i]);
        }
    }
    if (filterContours1.size() != 0)
        rectangle_blue = boundingRect(filterContours1[0]);
    
    for (size_t j = 1; j < filterContours1.size(); j++)
    {
        //画出轮廓所在矩形
        Rect rectangle_find = boundingRect(filterContours1[j]);//检测外轮廓
    }
    if (rectangle_blue.x > bwidth/2)
    {
        rectangle_blue_last = CircularQueue(rectangle_blue);
        int height = rectangle_blue_last.height/2;
        int width = rectangle_blue_last.width/2;
        rectangle_green = Rect(rectangle_blue_last.x+width/2,rectangle_blue_last.y+height/2,width,height);
        rectangle(drawing, rectangle_green, Scalar(0,255,0), 3);
    }
    
    findContours(mask2, contours2, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
    //去除伪轮廓
    for (size_t i = 0; i < contours2.size(); i++)
    {
        if (fabs(contourArea(Mat(contours2[i]))) > 300) //判断手进入区域的阈值
        {
            filterContours2.push_back(contours2[i]);
        }
    }
    
    if (filterContours2.size() != 0)
        rectangle_red = boundingRect(filterContours2[0]);
    
    //TODO:画轮廓
    for (size_t j = 0; j < filterContours2.size(); j++)
    {
        Rect rectangle_find = boundingRect(filterContours2[j]);//检测外轮廓
        if (rectangle_find.x + rectangle_find.width > rectangle_red.x + rectangle_red.width)
            rectangle_red = rectangle_find;
    }
    if (rectangle_red.x > bwidth/2)
    {
        rectangle(drawing, rectangle_red, Scalar(0,0,255), 3);//对外轮廓加矩形框(红色)
    }
    if (!isPut && O2::time_count > O2::time_cycle)//23秒内没加
    {
        //cout << "O型圈漏装" << endl;
        //putText(drawing, "ALARM", Point(10, 80), FONT_HERSHEY_SIMPLEX, 3, cvScalar(0,0,255),5);
        putTextZH(drawing, "漏装O型圈", Point(10, 10), Scalar(0, 0, 255), 100, "Arial");
        if (!isSound)
        {
            PlaySound(TEXT("C:/Users/健宁/Desktop/O2/O2/1.wav"), NULL, SND_FILENAME | SND_ASYNC );
            isSound = 1;
        }
        if (O2::time_count > O2::time_cycle+O2::time_show)//显示n秒警报后解除，重记时间
        {
            O2::time_count = O2::time_show+O2::time_notput_sub;
            isSound = 0;
        }
    }
    
    if (!isPut)//判断是否要放置
    {
        int x = rectangle_red.x + rectangle_red.width/2;
        int y = rectangle_red.y + rectangle_red.height/2;
        if (y > rectangle_green.y &&  x > rectangle_green.x && x < rectangle_green.x+rectangle_green.width && y < rectangle_green.y+rectangle_green.height)//如果放置了O圈
        {
            isPut = 1;
            isSound = 0;
            O2::time_count = 0;
        }
    }
    if (isPut && O2::time_count < O2::time_show)
    {
        //cout << "合格" << endl;
        //putText(drawing, "PASS", Point(10, 80), FONT_HERSHEY_SIMPLEX, 3, cvScalar(0,0,255),5);
        putTextZH(drawing, "合格", Point(10, 10), Scalar(0, 0, 255), 100, "Arial");
    }
    if (isPut && O2::time_count > O2::time_show)
    {
        isPut = 0;
    }
    return drawing;
}

int _tmain(int argc, _TCHAR* argv[])
{
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;
    O2 o = O2();
    VideoCapture capture;
    Mat frame;
    capture.open("C:\\Users\\健宁\\Desktop\\O2\\O2\\2.mp4");
    if (!capture.isOpened())
    {
        cout << "read video failed"<< endl;
        return -1;
    }
    cout << "read video success"<< endl;
    
    Mat mask(frame.size(), CV_8UC1);
    Mat mask1(frame.size(), CV_8UC1);
    Mat mask2(frame.size(), CV_8UC1);
    
    double t = 0 , fps = 0;
    char str[10];
    while(1)
    {
        t = getTickCount();
        if (waitKey(50) == 30) { break; }
        capture>>frame;
        
        fps = getTickFrequency() / (getTickCount() - t);
        //fps = 1.0 / t;
        sprintf(str, "%.2f", fps);
        std::string fpsString("FPS: ");
        fpsString += str;
        
        O2::time_count+=1;
        if(frame.empty())
        {
            cout << "frame.empty" << endl;
            break;
        }
        
        //旋转
        transpose(frame, frame);
        flip(frame, frame, 0);
        vector<vector<Point>> tmpContours;
        vector<vector<Point>> tmpFilterContours;
        vector<Vec4i> tmpHierarchy;
        Rect rectangle_find;
        
        //对HSV空间进行量化，得到2值图像
        //Mat element = getStructuringElement(MORPH_RECT, Size(3,3));//获取常用的结构元素的形状:矩形(包括线形)、椭圆(包括圆形)及十字形
        //erode(mask, mask, element);//腐蚀
        //morphologyEx(mask, mask, MORPH_OPEN, element); //开运算
        //dilate(mask, mask, element);//膨胀
        //morphologyEx(mask, mask, MORPH_CLOSE, element); //闭运算
        
        o.cvHSV(frame, mask1, mask2);
        frame = o.find(mask1, mask2, frame);
        putText(frame, fpsString, Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255));
        imshow("O型圈检测", frame);
        if(waitKey(10) >= 0)
            break;
    }
    
    return 0;
}
