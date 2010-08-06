// -*- mode: c++; c-basic-offset: 4; -*-

#include "onyx/application.h"

#include <QtGui/QtGui>

namespace onyx {

Application::Application() : q_app_(NULL) {}

Application::~Application() {}

void Application::initialize(int argc, char* argv[])
{
    initializeCustomStates(argc, argv);
    if (!q_app_) {
        q_app_ = new QApplication(argc, argv);
    }
}

int Application::onyxExec()
{
    int v = exec();
    if (v != 0) {
        return v;
    }
    return q_app_->exec();
}

void Application::setQApplicationInstance(QApplication* q_app)
{
    delete q_app_;
    q_app_ = q_app;
}

QApplication* Application::qApplicationInstance() {
    return q_app_;
}

}
