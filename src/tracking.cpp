#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <atomic>
#include <Python.h>

#include <opencv2/core/version.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

// includes for OpenCV >= 3.x
#ifndef CV_VERSION_EPOCH
#include <opencv2/core/types.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#endif

// OpenCV includes for OpenCV 2.x
#ifdef CV_VERSION_EPOCH
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/core/version.hpp>
#endif

using namespace cv;

class Tracking: public PyObject {
private:
    std::unordered_map<int,int> timer;
    unsigned short frame = 0;
    short allId = 0;
    int detects[6]={0};

    struct LostObject {
        float posx;
        float posy;
        short class_id;
        Mat hash;
        unsigned short time;
    };
    
    struct Object {
        float posx;
        float posy;
        float sizex;
        float sizey;
        int class_id;
        std::string hash;
    };

    struct box {
        float x;
        float y;
        float w;
        float h;
    }

    std::unordered_map<int,Object> objects;
    std::unordered_map<int,LostObject> lostObjects;
    std::unordered_map<int,Object> newObjects;

    Mat hashImage(Mat image) {
        Mat img;
        resize(image, img, Size(8, 8), INTER_LINEAR);
        cvtColor(img, image, COLOR_BGR2GRAY);
        threshold(image, img, sum(mean(image))[0], 255, 0);
        return img;
    }

    int compareHash(Mat hash1, Mat hash2) {
        int diff = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                auto val1 = hash1.at<uchar>(i, j);
                auto val2 = hash2.at<uchar>(i, j);
                if (val1 != val2) {
                    diff++;
                }
            }
        }
        return diff;
    }

public:
    void track(Mat *mat, PyObject* bbox, PyObject* scores, PyObject* classes, int num, PyObject* thresh) {
        Mat img = mat;
        if (img.empty()) return;
        for (int i = 0; i < num; i++) {
            if(PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(bbox, i),3))!=0.0 && PyFloat_AsDouble(PyList_GetItem(thresh, i))<=PyFloat_AsDouble(PyList_GetItem(scores, PyLong_AsLong(PyList_GetItem(classes, i))))){
                box b;
                
                if (std::isnan(b.w) || std::isinf(b.w)) b.w = 0.5;
                if (std::isnan(b.h) || std::isinf(b.h)) b.h = 0.5;
                if (std::isnan(b.x) || std::isinf(b.x)) b.x = 0.5;
                if (std::isnan(b.y) || std::isinf(b.y)) b.y = 0.5;
                b.w = (b.w < 1) ? b.w : 1;
                b.h = (b.h < 1) ? b.h : 1;
                b.x = (b.x < 1) ? b.x : 1;
                b.y = (b.y < 1) ? b.y : 1;

                Rect r;
                r.x=int(b.x*img.cols);
                r.y=int(b.y*img.rows);
                r.width=int(b.w*img.cols);
                if(r.x+r.width>=img.cols){
                r.width=img.cols-r.x-1;
                }
                r.height=int(b.h*img.rows);
                if(r.y+r.height>=img.rows){
                r.height=img.rows-r.y-1;
                }
                Mat cr = img(r);
                Mat hash = hashImage(&cr);
                int temp;
                int min = -1;
                int id = -1;
                for (std::pair<int,Object> obj : objects) {
                    if (obj.second.class_id == class_id && abs(b.x - obj.second.posx) <= b.w*0.6 && abs(b.y - obj.second.posy) <= b.h*0.6) {
                        temp = (pow((abs(b.x - obj.second.posx)*img.cols + abs(b.y - obj.second.posy)*img.rows),2)) * 10 + compareHash(hash,obj.second.hash) * 5;
                        if (min == -1) {
                            min = temp;
                        }
                        if (temp <= min && newObjects.find(obj.first)==newObjects.end()) {
                            min = temp;
                            id = obj.first;
                        }
                    }
                }
                if (id == -1) {
                    for (std::pair<int,LostObject> obj : lostObjects) {
                        if (class_id == obj.second.class_id && compareHash(hash, obj.second.hash)<25 && abs(b.x - obj.second.posx) + abs(b.y - obj.second.posy) < 0.03) {
                            id = obj.first;
                            lostObjects.erase(obj.first);
                            break;
                        }
                    }
                }
                if (id == -1) {
                    id=allId+1;
                    allId+=1;
                }
                newObjects.insert({id,{b.x,b.y,b.w,b.h,class_id,hash}});
            }
        }
        for (std::pair<int,Object> obj : objects) {
            if (newObjects.find(obj.first)!=newObjects.end()) {
                lostObjects.insert({ obj.first, {obj.second.posx, obj.second.posy, obj.second.class_id, obj.second.hash, 0} });
            }
            if (timer.find(obj.first)!=timer.end()){
                timer.insert({obj.first,0});
            }
            if (timer[obj.first]==20){
                detects[obj.second.class_id]+=1;
            }
            timer[obj.first]+=1;
        }
        objects=newObjects;
        newObjects.clear();
        std::list<int> delObjects;
        for (std::pair<int,LostObject> obj : lostObjects) {
            obj.second.time += 1;
            if (obj.second.time == 180) {
                delObjects.insert(delObjects.end(),obj.first);
            }
        }
        for (int id : delObjects) {
            lostObjects.erase(id);
        }
    };
};