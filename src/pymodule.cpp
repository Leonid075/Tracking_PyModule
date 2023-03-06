#include <iostream>
#include <Python.h>

#include <opencv2/core/version.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>


using namespace cv;

static PyObject* spam_system(PyObject* self, PyObject* args)
{
    char* cfg;
    char* datacfg;
    char* weights;
    float thresh;
    char* filename;
    char* out_filename;
    int dontdraw_bbox;
    PyObject* out;


    PyArg_ParseTuple(args, "O|O:ref", &out);
    //PyArg_ParseTuple(args, "sssfssi", &cfg, &datacfg, &weights, &thresh, &filename, &out_filename, &dontdraw_bbox);
    //demo(cfg, weights, thresh, .5, 0, filename, 0, classes, 3, 0, 0, out_filename, -1, dontdraw_bbox, -1, 1, 0, 0, 0, 0, 0, 0);
    //long i = PyLong_AsLong();
    //PyObject* j = PyLong_FromLong(i);
    Mat img(416, 416, CV_8UC3, (uchar*)PyByteArray_AsString(out));
    return PyLong_FromLong(123);
}

static PyMethodDef SpamMethods[] = {
    {"system",  spam_system, METH_VARARGS, "Execute a shell command."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",//module name
    NULL,
    -1,
    SpamMethods//methods
};

static PyObject* SpamError;
PyMODINIT_FUNC
PyInit_spam(void)
{
    PyObject* m;
    m = PyModule_Create(&spammodule);
    if (m == NULL)
        return NULL;
    return m;
}