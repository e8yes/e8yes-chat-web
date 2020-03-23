package org.e8yes.service;

import io.grpc.stub.StreamObserver;

/** @author davis */
public class SocialNetworkService extends SocialNetworkServiceGrpc.SocialNetworkServiceImplBase {

  @Override
  public void sendFriendshipRequest(FriendshipRRequest req, StreamObserver<FriendshipRReply> res) {}

  @Override
  public void acceptFriendRequest(
      AcceptFriendshipRRequest req, StreamObserver<AcceptFriendshipRReply> res) {}

  @Override
  public void removeFriend(RemoveFriendRequest req, StreamObserver<RemoveFriendReply> res) {}

  @Override
  public void getFriendList(FriendListRequest req, StreamObserver<FriendListReply> res) {}
}