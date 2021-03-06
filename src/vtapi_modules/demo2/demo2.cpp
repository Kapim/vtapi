#include <thread>
#include <chrono>
#include <Poco/ClassLibrary.h>
#include <vtapi/common/logger.h>
#include "demo2.h"

using namespace std;

namespace vtapi {


Demo2Module::Demo2Module()
{
    _stop = false;
}

Demo2Module::~Demo2Module()
{

}

void Demo2Module::initialize(VTApi & vtapi)
{
    VTLOG_MESSAGE("demo2: initializing");
}

void Demo2Module::uninitialize() noexcept
{
    VTLOG_MESSAGE("demo2 : uninitializing");
}

void Demo2Module::process(Process & process)
{
    VTLOG_MESSAGE("demo2 : process=" + vtapi::toString<int>(process.getId()));

    //TODO: demo module is very basic

    {
        shared_ptr<Task> task (process.getParentTask());
        if (task) {
            VTLOG_MESSAGE("demo2 : task=" + task->getName());
        }
    }

    {
        shared_ptr<Video> video(process.loadAssignedVideos());
        while(video->next() && !_stop) {
            VTLOG_MESSAGE("demo2 : analyzing " + video->getName());
            this_thread::sleep_for(chrono::milliseconds(2500));
        }
    }

    VTLOG_MESSAGE("demo2 : processing stopped");
}

void Demo2Module::stop() noexcept
{
    _stop = true;
}


}


POCO_BEGIN_MANIFEST(vtapi::IModuleInterface)
    POCO_EXPORT_CLASS(vtapi::Demo2Module)
POCO_END_MANIFEST
