/*
 *  This file is a part of KNOSSOS.
 *
 *  (C) Copyright 2007-2016
 *  Max-Planck-Gesellschaft zur Foerderung der Wissenschaften e.V.
 *
 *  KNOSSOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 of
 *  the License as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  For further information, visit https://knossostool.org
 *  or contact knossos-team@mpimf-heidelberg.mpg.de
 */

#ifndef LOADER_H
#define LOADER_H

#include "dataset.h"
#include "hashtable.h"
#include "segmentation/segmentation.h"

#include <QCoreApplication>
#include <QFutureWatcher>
#include <QMutex>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>
#include <QSemaphore>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QWaitCondition>

#include <boost/multi_array.hpp>

#include <atomic>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/* Calculate movement trajectory for loading based on how many last single movements */
#define LL_CURRENT_DIRECTIONS_SIZE (20)
/* Max number of metrics allowed for sorting loading order */
#define LL_METRIC_NUM (20)

#define LM_LOCAL    0
#define LM_FTP      1

bool currentlyVisibleWrapWrap(const Coordinate & center, const Coordinate & coord);

namespace Loader{
class Controller;
class Worker;
}

namespace Loader {
class Worker;

class Worker : public QObject {
    Q_OBJECT
    friend class Loader::Controller;
    friend boost::multi_array_ref<uint64_t, 3> getCube(const Coordinate & pos);
    friend void Segmentation::clear();
private:
    QThreadPool decompressionPool;//let pool be alive just after ~Worker
    QNetworkAccessManager qnam;

    template<typename T>
    using ptr = std::unique_ptr<T>;
    using DecompressionResult = std::pair<bool, char*>;
    using DecompressionOperationPtr = ptr<QFutureWatcher<DecompressionResult>>;
    std::unordered_map<Coordinate, QNetworkReply*> dcDownload;
    std::unordered_map<Coordinate, QNetworkReply*> ocDownload;
    std::unordered_map<Coordinate, DecompressionOperationPtr> dcDecompression;
    std::unordered_map<Coordinate, DecompressionOperationPtr> ocDecompression;
    std::list<std::vector<char>> DcSetChunk;
    std::list<std::vector<char>> OcSetChunk;
    std::list<char*> freeDcSlots;
    std::list<char*> freeOcSlots;
    int currentMaxMetric;

    std::atomic_bool isFinished{false};
    uint loaderMagnification = 0;
    void CalcLoadOrderMetric(float halfSc, floatCoordinate currentMetricPos, floatCoordinate direction, float *metrics);
    floatCoordinate find_close_xyz(floatCoordinate direction);
    std::vector<CoordOfCube> DcoiFromPos(const Coordinate &center);
    uint loadCubes();
    void snappyCacheBackupRaw(const CoordOfCube &, const char *cube);
    void snappyCacheClear();

    void abortDownloadsFinishDecompression();
    template<typename Func>
    void abortDownloadsFinishDecompression(Func);

    const QUrl baseUrl;
    const Dataset::API api;
    const Dataset::CubeType typeDc;
    const Dataset::CubeType typeOc;
    const QString experimentName;
public://matsch
    using CacheQueue = std::unordered_set<CoordOfCube>;
    std::vector<CacheQueue> OcModifiedCacheQueue;
    using SnappyCache = std::unordered_map<CoordOfCube, std::string>;
    std::vector<SnappyCache> snappyCache;
    QMutex snappyMutex;
    QWaitCondition snappyFlushCondition;

    void moveToThread(QThread * targetThread);//reimplement to move qnam

    void unloadCurrentMagnification();
    void markOcCubeAsModified(const CoordOfCube &cubeCoord, const int magnification);
    void snappyCacheSupplySnappy(const CoordOfCube, const int magnification, const std::string cube);
    void flushIntoSnappyCache();
    void broadcastProgress(bool startup = false);
    void allocateOverlayCubes();
    Worker(const QUrl & baseUrl, const Dataset::API api, const Dataset::CubeType typeDc, const Dataset::CubeType typeOc, const QString & experimentName);
    ~Worker();
signals:
    void progress(bool incremented, int count);
public slots:
    void cleanup(const Coordinate center);
    void downloadAndLoadCubes(const unsigned int loadingNr, const Coordinate center);
};

class Controller : public QObject {
    Q_OBJECT
    friend class Loader::Worker;
    QThread workerThread;
public:
    std::unique_ptr<Loader::Worker> worker;
    std::atomic_uint loadingNr{0};
    static Controller & singleton(){
        static Loader::Controller loader;
        return loader;
    }
    void suspendLoader();
    ~Controller();
    void unloadCurrentMagnification();
    void enableOverlay() {
        suspendLoader();
        worker->allocateOverlayCubes();
        workerThread.start();
    }

    template<typename... Args>
    void restart(Args&&... args) {
        suspendLoader();
        if (worker != nullptr) {
            worker->flushIntoSnappyCache();
            auto snappyCache = worker->snappyCache;
            worker.reset(new Loader::Worker(std::forward<Args>(args)...));
            worker->snappyCache = snappyCache;
        } else {
            worker.reset(new Loader::Worker(std::forward<Args>(args)...));
        }
        workerThread.setObjectName("Loader");
        worker->moveToThread(&workerThread);
        QObject::connect(worker.get(), &Loader::Worker::progress, this, [this](bool, int count){emit progress(count);});
        QObject::connect(worker.get(), &Loader::Worker::progress, this, &Loader::Controller::refCountChange);
        QObject::connect(this, &Loader::Controller::loadSignal, worker.get(), &Loader::Worker::downloadAndLoadCubes);
        QObject::connect(this, &Loader::Controller::unloadCurrentMagnificationSignal, worker.get(), &Loader::Worker::unloadCurrentMagnification, Qt::BlockingQueuedConnection);
        QObject::connect(this, &Loader::Controller::markOcCubeAsModifiedSignal, worker.get(), &Loader::Worker::markOcCubeAsModified, Qt::BlockingQueuedConnection);
        QObject::connect(this, &Loader::Controller::snappyCacheSupplySnappySignal, worker.get(), &Loader::Worker::snappyCacheSupplySnappy, Qt::BlockingQueuedConnection);
        workerThread.start();
    }
    void startLoading(const Coordinate &center);
    template<typename... Args>
    void snappyCacheSupplySnappy(Args&&... args) {
        emit snappyCacheSupplySnappySignal(std::forward<Args>(args)...);
    }
    void markOcCubeAsModified(const CoordOfCube &cubeCoord, const int magnification);
    decltype(Loader::Worker::snappyCache) getAllModifiedCubes();
public slots:
    bool isFinished();
signals:
    void progress(int count);
    void refCountChange(bool isIncrement, int refCount);
    void unloadCurrentMagnificationSignal();
    void loadSignal(const unsigned int loadingNr, const Coordinate center);
    void markOcCubeAsModifiedSignal(const CoordOfCube &cubeCoord, const int magnification);
    void snappyCacheSupplySnappySignal(const CoordOfCube, const int magnification, const std::string cube);
};
}//namespace Loader

#endif//LOADER_H
