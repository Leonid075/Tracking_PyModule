#include <Python.h>

class Tracking: public PyObject {
    public:
    Tracking(short num, PyObject* threshs, ushort d_time, ushort a_time);
    void track(PyObject* img, PyObject* bbox, PyObject* scores, PyObject* classes);
};