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
    unsigned short frame = 0;
    short allId = 0;

    short detects[6]={0}; //vector

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
        short class_id;
        Mat hash;
        unsigned short time;
    };

    std::unordered_map<short, short> timer;
    std::unordered_map<short, Object> objects;
    std::unordered_map<short, LostObject> lostObjects;

    Mat hashImage(Mat image) {
        Mat img;
        resize(image, img, Size(8, 8), INTER_LINEAR);
        cvtColor(img, image, COLOR_BGR2GRAY);
        threshold(image, img, sum(mean(image))[0], 255, 0);
        return img;
    }

    short compareHash(Mat* hash1, Mat* hash2) {
        int diff = 0;
        for (short i = 0; i < 8; i++) {
            for (short j = 0; j < 8; j++) {
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
    Tracking()

    void track(Mat *img, PyObject* bbox, PyObject* scores, PyObject* classes, short num, float* thresh) {
        if (img.empty()) return;
        std::unordered_map<short, Object> newObjects;
        for (short i = 0; i < num; i++) {
            float w = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(bbox, i), 3));
            float h = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(bbox, i), 2));

            if(w<0.02 || h<0.02){ break; }
            
            float x = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(bbox, i), 0));
            float y = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(bbox, i), 1));
            short class_id = PyLong_AsLong(PyList_GetItem(classes, i));

            if(thresh[class_id] > PyFloat_AsDouble(PyList_GetItem(scores, i))){ break; }
            /*if (std::isnan(b.w) || std::isinf(b.w)) b.w = 0.5;
            if (std::isnan(b.h) || std::isinf(b.h)) b.h = 0.5;
            if (std::isnan(b.x) || std::isinf(b.x)) b.x = 0.5;
            if (std::isnan(b.y) || std::isinf(b.y)) b.y = 0.5;
            b.w = (b.w < 1) ? b.w : 1;
            b.h = (b.h < 1) ? b.h : 1;
            b.x = (b.x < 1) ? b.x : 1;
            b.y = (b.y < 1) ? b.y : 1;*/

            Rect r;
            r.x=int(x*img.cols);
            r.y=int(y*img.rows);
            r.width=int(w*img.cols);
            r.height=int(h*img.rows);

            int temp;
            int min = -1;
            short id = -1;
            Mat hash = hashImage(img(r));
            for (std::pair<short, Object> obj : objects) {
                if (obj.second.class_id == class_id && abs(x - obj.second.posx) <= w*0.6 && abs(y - obj.second.posy) <= h*0.6) {
                    temp = (pow((abs(x - obj.second.posx)*img.cols + abs(y - obj.second.posy)*img.rows),2)) * 10 + compareHash(&hash, &obj.second.hash) * 5;
                    min = (min == -1) ? temp : min;
                    if (temp <= min && newObjects.find(obj.first)==newObjects.end()) {
                        min = temp;
                        id = obj.first;
                    }
                }
            }
            if (id == -1) {
                for (std::pair<short, LostObject> obj : lostObjects) {
                    if (class_id == obj.second.class_id && compareHash(&hash, &obj.second.hash)<25 && abs(x - obj.second.posx) + abs(y - obj.second.posy) < 0.03) {
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
            newObjects.insert({id, {x, y, w, h, class_id, hash}});            
        }

        for (std::pair<short,Object> obj : objects) {
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

        std::list<short> delObjects;
        for (std::pair<short,LostObject> obj : lostObjects) {
            obj.second.time += 1;
            if (obj.second.time == 180) {
                delObjects.insert(delObjects.end(), obj.first);
            }
        }
        for (short id : delObjects) {
            lostObjects.erase(id);
        }
    };
};