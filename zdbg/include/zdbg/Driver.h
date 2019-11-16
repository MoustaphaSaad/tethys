#pragma once

#include <QtCore/QObject>
#include <QtQml/QQmlApplicationEngine>

namespace zdbg
{
    class Driver: public QObject
    {
        Q_OBJECT
    public:
        Driver(QObject* parent = Q_NULLPTR);
        ~Driver();

        Q_INVOKABLE void loadQml();
        void setEngine(QQmlApplicationEngine* engine) { mQmlEngine = engine; }
    private:
        QQmlApplicationEngine* mQmlEngine;
    };
}
