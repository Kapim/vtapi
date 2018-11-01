/*
 * VideoTerror server
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz), Tomas Volf (ivolf[at]fit.vutbr.cz)
 */


#pragma once

#include "worker.h"
#include "interproc.h"
#include <vtapi/vtapi.h>
#include <grpcpp/grpcpp.h>
#include "vtserver_interface.grpc.pb.h"
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

using grpc::Status;
using grpc::ServerContext;

namespace vtserver {


 class VTServer : public vtserver_interface::VTServerInterface::Service
 {
 public:
    VTServer(const vtapi::VTApi &vtapi);

    // VTServerInterface interface

    Status addDataset(ServerContext* context, const vtserver_interface::addDatasetRequest* request, vtserver_interface::addDatasetResponse* response);
    Status getDatasetList(ServerContext* context, const vtserver_interface::getDatasetListRequest* request, vtserver_interface::getDatasetListResponse* response);
    Status getDatasetMetrics(ServerContext* context, const vtserver_interface::getDatasetMetricsRequest* request, vtserver_interface::getDatasetMetricsResponse* response);
    Status deleteDataset(ServerContext* context, const vtserver_interface::deleteDatasetRequest* request, vtserver_interface::deleteDatasetResponse* response);
    Status addSequence(ServerContext* context, const vtserver_interface::addSequenceRequest* request, vtserver_interface::addSequenceResponse* response);
    Status getSequenceIDList(ServerContext* context, const vtserver_interface::getSequenceIDListRequest* request, vtserver_interface::getSequenceIDListResponse* response);
    Status getSequenceInfo(ServerContext* context, const vtserver_interface::getSequenceInfoRequest* request, vtserver_interface::getSequenceInfoResponse* response);
    Status setSequenceInfo(ServerContext* context, const vtserver_interface::setSequenceInfoRequest* request, vtserver_interface::setSequenceInfoResponse* response);
    Status deleteSequence(ServerContext* context, const vtserver_interface::deleteSequenceRequest* request, vtserver_interface::deleteSequenceResponse* response);
    Status addTask(ServerContext* context, const vtserver_interface::addTaskRequest* request, vtserver_interface::addTaskResponse* response);
    Status getTaskIDList(ServerContext* context, const vtserver_interface::getTaskIDListRequest* request, vtserver_interface::getTaskIDListResponse* response);
    Status getTaskInfo(ServerContext* context, const vtserver_interface::getTaskInfoRequest* request, vtserver_interface::getTaskInfoResponse* response);
    Status getTaskProgress(ServerContext* context, const vtserver_interface::getTaskProgressRequest* request, vtserver_interface::getTaskProgressResponse* response);
    Status deleteTask(ServerContext* context, const vtserver_interface::deleteTaskRequest* request, vtserver_interface::deleteTaskResponse* response);
    Status getProcessIDList(ServerContext* context, const vtserver_interface::getProcessIDListRequest* request, vtserver_interface::getProcessIDListResponse* response);
    Status getProcessInfo(ServerContext* context, const vtserver_interface::getProcessInfoRequest* request, vtserver_interface::getProcessInfoResponse* response);
    Status runProcess(ServerContext* context, const vtserver_interface::runProcessRequest* request, vtserver_interface::runProcessResponse* response);
    Status stopProcess(ServerContext* context, const vtserver_interface::stopProcessRequest* request, vtserver_interface::stopProcessResponse* response);
    Status getEventImage(ServerContext* context, const vtserver_interface::getEventImageRequest* request, vtserver_interface::getEventImageResponse* response);
    Status getEventDescriptor(ServerContext* context, const vtserver_interface::getEventDescriptorRequest* request, vtserver_interface::getEventDescriptorResponse* response);
    Status getEventList(ServerContext* context, const vtserver_interface::getEventListRequest* request, vtserver_interface::getEventListResponse* response);
    Status getEventsStats(ServerContext* context, const vtserver_interface::getEventsStatsRequest* request, vtserver_interface::getEventsStatsResponse* response);
    Status getProcessingMetadata(ServerContext* context, const vtserver_interface::getProcessingMetadataRequest* request, vtserver_interface::getProcessingMetadataResponse* response);

private:
    std::mutex _mtx_threads;
    std::map<std::thread::id,int> _thread_indexes;
    std::vector< std::shared_ptr<vtapi::VTApi> > _connections;
    Interproc _interproc;

    template<class REQUEST_T, class RESPONSE_T>
    bool processRequest(REQUEST_T* request, RESPONSE_T* reply);
 };


}
