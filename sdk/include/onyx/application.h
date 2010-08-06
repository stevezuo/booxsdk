// -*- mode: c++; c-basic-offset: 4; -*-

#ifndef ONYX_APPLICATION_H_
#define ONYX_APPLICATION_H_

#include <QtGui/QtGui>

#include "onyx/base/macros.h"

namespace onyx {

class Application {
  public:
    Application();
    virtual ~Application();
    void initialize(int argc, char* argv[]);
    // This function is called by initialize() as the first thing. If
    // you want to do some custom initialization, such as injecting
    // your own subclass of QApplication, this is the right place.
    virtual void initializeCustomStates(int argc, char* argv[]) {}
    virtual int exec() = 0;
    int onyxExec();

  protected:
    // The injected QApplication object is owned by this object. Do
    // not delete elsewhere.
    void setQApplicationInstance(QApplication* q_app);
    QApplication* qApplicationInstance();

  private:
    QApplication* q_app_;
    NO_COPY_AND_ASSIGN(Application);
};

extern Application* g_onyx_application;

}  // namespace onyx

// This macro should be used in a C++ source file (not header file) to
// initialize the global Application instance.
#define ONYX_APP_CLASS(classname) \
    namespace onyx { \
    Application* g_onyx_application = new classname; \
    }

#endif  // ONYX_APPLICATION_H_
