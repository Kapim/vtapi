import grpc
import vtserver_interface_pb2
import vtserver_interface_pb2_grpc
from protodict import to_dict, to_protobuf
from google.protobuf.message import EncodeError


class VTServerException(Exception):
    pass


class VTServerClient(object):

    def __init__(self, conn_str, deadline_ms=5000):

        self.conn_str = conn_str
        self.deadline_ms = deadline_ms

    def call(self, service, req, deadline_ms=None):

        if deadline_ms is None:
            deadline_ms = self.deadline_ms

        try:
            request = getattr(vtserver_interface_pb2, service + "Request")()
        except AttributeError:
            raise VTServerException('Service "' + service + '" does not exist.')

        with grpc.insecure_channel(
                target=self.conn_str,
                options=[('grpc.lb_policy_name', 'pick_first'),
                         ('grpc.enable_retries', 0), ('grpc.keepalive_timeout_ms',
                                                      10000)]) as channel:

            stub = vtserver_interface_pb2_grpc.VTServerInterfaceStub(channel)

            srv_method = getattr(stub, service)

            try:
                response = srv_method(to_protobuf(request, req), timeout = deadline_ms/1000.0)
            except AttributeError:
                print "[Client error]: Request must be a dictionary."
                raise VTServerException('Request must be a dictionary.')
            except EncodeError:
                print "[Client error]: Request is not correct (some required field is missing?)."
                raise VTServerException('Request is not correct (some required field is missing?).')
            except grpc.FutureTimeoutError:
                print "[Client error]: No answer from server in given time."
                return {'res': {'success': False, 'error': 'No answer from server in given time.'}}
                #raise VTServerException('No answer from server in given time.')

            return to_dict(response)
