///
//  Generated code. Do not modify.
//  source: service_node_info.proto
//
// @dart = 2.3
// ignore_for_file: camel_case_types,non_constant_identifier_names,library_prefixes,unused_import,unused_shown_name,return_of_invalid_type

import 'dart:async' as $async;

import 'dart:core' as $core;

import 'package:grpc/service_api.dart' as $grpc;
import 'service_node_info.pb.dart' as $3;
export 'service_node_info.pb.dart';

class NodeStateServiceClient extends $grpc.Client {
  static final _$updateNodeState =
      $grpc.ClientMethod<$3.UpdateNodeStateRequest, $3.UpdateNodeStateResponse>(
          '/e8.NodeStateService/UpdateNodeState',
          ($3.UpdateNodeStateRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) =>
              $3.UpdateNodeStateResponse.fromBuffer(value));

  NodeStateServiceClient($grpc.ClientChannel channel,
      {$grpc.CallOptions options})
      : super(channel, options: options);

  $grpc.ResponseFuture<$3.UpdateNodeStateResponse> updateNodeState(
      $3.UpdateNodeStateRequest request,
      {$grpc.CallOptions options}) {
    final call = $createCall(
        _$updateNodeState, $async.Stream.fromIterable([request]),
        options: options);
    return $grpc.ResponseFuture(call);
  }
}

abstract class NodeStateServiceBase extends $grpc.Service {
  $core.String get $name => 'e8.NodeStateService';

  NodeStateServiceBase() {
    $addMethod($grpc.ServiceMethod<$3.UpdateNodeStateRequest,
            $3.UpdateNodeStateResponse>(
        'UpdateNodeState',
        updateNodeState_Pre,
        false,
        false,
        ($core.List<$core.int> value) =>
            $3.UpdateNodeStateRequest.fromBuffer(value),
        ($3.UpdateNodeStateResponse value) => value.writeToBuffer()));
  }

  $async.Future<$3.UpdateNodeStateResponse> updateNodeState_Pre(
      $grpc.ServiceCall call,
      $async.Future<$3.UpdateNodeStateRequest> request) async {
    return updateNodeState(call, await request);
  }

  $async.Future<$3.UpdateNodeStateResponse> updateNodeState(
      $grpc.ServiceCall call, $3.UpdateNodeStateRequest request);
}