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

#include "tracking.h"

using namespace cv;

static PyObject* spam_system(PyObject* self, PyObject* args){
    PyObject* out;
    PyArg_ParseTuple(args, "O|O:ref", &out);
    return PyLong_FromLong(123);
}

static PyObject* new_track(PyObject* self, PyObject* args){
    short* num;
    PyObject* img;
    ushort* d_time;
    ushort* a_time;
    PyArg_ParseTuple(args, "iOii", &num, &img, &d_time, &a_time);
    Tracking tr(*num, img, *d_time, *a_time);
    return Py_BuildValue("O", &tr);
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