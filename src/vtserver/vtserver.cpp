// VTServer application - main entry
// by ifroml[at]fit.vutbr.cz, ivolf[at]fit.vutbr.cz
//
// Service acts as main interface for computer vision applications based on VTApi,
// listening on port 8719.
// Interface is defined in vtserver_interface.proto (Protocol Buffers format).
//
// worker.cpp       main interface implementation
// interproc.cpp    interprocess communication to manage active processing tasks
// sequencestats.cpp   calculation of statistics for sequence from processing results
// vtserver_interface*  generated interface files
//
// Dataset interface
// - manage datasets (collection of videos, tasks and processing results)
// - methods: add, delete, get info
//
// Video interface
// - manipulate videos in a dataset
// - methods: add, delete, get/set info
//
// Task interface
// - first define, then run processing tasks and later query results (Events interface)
// - methods: add, delete, get info, get progress, run process, stop process
//
// Events interface
// - query results of finished tasks
// - methods: get list, get stats


#include "vtserver.h"
#include <iostream>

#define WORKER_THREAD_COUNT     10      // concurrent worker threads
#define ZEROMQ_IO_THREAD_COUNT  1       // IO threads

namespace vti = vtserver_interface;

using grpc::Status;
using grpc::ServerBuilder;
using grpc::Server;


int main(int argc, char *argv[])
{
    try {
        // main vtapi object with connection
        vtapi::VTApi vtapi(argc, argv);
        // initialize interface, copy vtapi object to all worker threads
        vtserver::VTServer vtserver(vtapi);

        std::string server_address("0.0.0.0:8719");

        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *synchronous* service.
        builder.RegisterService(&vtserver);
        // Finally assemble the server.
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();

    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////
//                      VTServer implementation                      //
///////////////////////////////////////////////////////////////////////


namespace vtserver {


VTServer::VTServer(const vtapi::VTApi &vtapi)
    : _connections(WORKER_THREAD_COUNT)
{
    for (auto & conn : _connections)
        conn = std::make_shared<vtapi::VTApi>(vtapi);
}


template<class REQUEST_T, class RESPONSE_T>
bool VTServer::processRequest(REQUEST_T* request, RESPONSE_T* response)
{
    // find current thread index

    int thread_index = -1;
    std::thread::id thread_id = std::this_thread::get_id();

    _mtx_threads.lock();
    auto ti = _thread_indexes.find(thread_id);
    if (ti != _thread_indexes.end()) {
        thread_index = ti->second;
    }
    else {
        thread_index = _thread_indexes.size();
        _thread_indexes.emplace(std::make_pair(thread_id, thread_index));
    }
    _mtx_threads.unlock();

    // get connection for our thread and process request

    if (thread_index >= 0 && thread_index < WORKER_THREAD_COUNT) {
        WorkerJob<REQUEST_T,RESPONSE_T> job(*request, *response);
        WorkerJobBase::Args args(*_connections[thread_index], _interproc);
        job.process(args);
    }
}


Status VTServer::addDataset(ServerContext* context, const vti::addDatasetRequest* request, vti::addDatasetResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getDatasetList(ServerContext* context, const vti::getDatasetListRequest* request, vti::getDatasetListResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getDatasetMetrics(ServerContext* context, const vti::getDatasetMetricsRequest* request, vti::getDatasetMetricsResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::deleteDataset(ServerContext* context, const vti::deleteDatasetRequest* request, vti::deleteDatasetResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::addSequence(ServerContext* context, const vti::addSequenceRequest* request, vti::addSequenceResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getSequenceIDList(ServerContext* context, const vti::getSequenceIDListRequest* request, vti::getSequenceIDListResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getSequenceInfo(ServerContext* context, const vti::getSequenceInfoRequest* request, vti::getSequenceInfoResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::setSequenceInfo(ServerContext* context, const vti::setSequenceInfoRequest* request, vti::setSequenceInfoResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::deleteSequence(ServerContext* context, const vti::deleteSequenceRequest* request, vti::deleteSequenceResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::addTask(ServerContext* context, const vti::addTaskRequest* request, vti::addTaskResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getTaskIDList(ServerContext* context, const vti::getTaskIDListRequest* request, vti::getTaskIDListResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getTaskInfo(ServerContext* context, const vti::getTaskInfoRequest* request, vti::getTaskInfoResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getTaskProgress(ServerContext* context, const vti::getTaskProgressRequest* request, vti::getTaskProgressResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::deleteTask(ServerContext* context, const vti::deleteTaskRequest* request, vti::deleteTaskResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getProcessIDList(ServerContext* context, const vti::getProcessIDListRequest* request, vti::getProcessIDListResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getProcessInfo(ServerContext* context, const vti::getProcessInfoRequest* request, vti::getProcessInfoResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::runProcess(ServerContext* context, const vti::runProcessRequest* request, vti::runProcessResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::stopProcess(ServerContext* context, const vti::stopProcessRequest* request, vti::stopProcessResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getEventImage(ServerContext* context, const vti::getEventImageRequest* request, vti::getEventImageResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getEventDescriptor(ServerContext* context, const vti::getEventDescriptorRequest* request, vti::getEventDescriptorResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getEventColorDescriptor(ServerContext* context, const vti::getEventColorDescriptorRequest* request, vti::getEventColorDescriptorResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getEventList(ServerContext* context, const vti::getEventListRequest* request, vti::getEventListResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getEventsStats(ServerContext* context, const vti::getEventsStatsRequest* request, vti::getEventsStatsResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}

Status VTServer::getProcessingMetadata(ServerContext* context, const vti::getProcessingMetadataRequest* request, vti::getProcessingMetadataResponse* response)
{
    processRequest(request, response);
    return Status::OK;
}



}


