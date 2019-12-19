#include <Python.h>

#include <QDebug>

#include <QCommandLineParser>
#include <QStandardPaths>
#include <QMainWindow>
#include <QCoreApplication>
#include <QSurfaceFormat>
#include <QTextCodec>
#include <QStringList>
#include <QMessageBox>
#include <iostream>

#include "app/app.h"
#include "graph/hooks/hooks.h"

#include "fab/fab.h"
#include "graph/graph.h"
#include "graph/util.h"

int main(int argc, char *argv[])
{
  Py_Initialize();

  // Swap in a stringIO object for stdout, saving stdout in out
  PyObject* sys_mod = PyImport_ImportModule("sys");
  PyObject* io_mod = PyImport_ImportModule("io");
  PyObject* stdout_obj = PyObject_GetAttrString(sys_mod, "stdout");
  PyObject* stderr_obj = PyObject_GetAttrString(sys_mod, "stderr");
  PyObject* string_out = PyObject_CallMethod(io_mod, "StringIO", NULL);
  PyObject_SetAttrString(sys_mod, "stdout", string_out);
  PyObject_SetAttrString(sys_mod, "stderr", string_out);

  // Run the script
  //why run multiple times?
  std::string script =
      "import os\n"
      "path = '.'\n"
      "for file in os.listdir(path):\n"
      "    print(file)\n";
  PyObject* globals = Py_BuildValue(
          "{sO}", "__builtins__", PyEval_GetBuiltins());
  PyObject* out = PyRun_String(
          script.c_str(), Py_file_input, globals, globals);
  Py_XDECREF(out);

  if (PyErr_Occurred())
  {
    std::string error;
    int error_lineno;
      auto err = getPyError();
      error = err.first;
      error_lineno = err.second;
      PyErr_Clear();
      std::cerr<<"python error: "<<error<<" at line "<<error_lineno<<std::endl;
  }



  // Get the output from the StringIO object
  PyObject* s = PyObject_CallMethod(string_out, "getvalue", NULL);
  std::string output = std::string(PyUnicode_AsUTF8(s));
  std::cerr<<"python output: "<<output<<std::endl;
}
