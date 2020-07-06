import "dart:async";

import 'package:angular/angular.dart';
import 'package:angular_forms/angular_forms.dart';
import 'package:angular_router/angular_router.dart';
import 'package:demoweb_app/src/context.dart';
import 'package:demoweb_app/src/proto_dart/pagination.pbserver.dart';
import 'package:demoweb_app/src/proto_dart/nullable_primitives.pb.dart';
import 'package:demoweb_app/src/proto_dart/service_socialnetwork.pb.dart';
import 'package:demoweb_app/src/proto_dart/service_user.pb.dart';
import 'package:demoweb_app/src/proto_dart/user_profile.pb.dart';
import 'package:demoweb_app/src/proto_dart/user_relation.pb.dart';
import 'package:demoweb_app/src/route_paths.dart';
import 'package:demoweb_app/src/socialnetwork_service_interface.dart';
import 'package:demoweb_app/src/sync_search_result.dart';
import 'package:demoweb_app/src/user_service_interface.dart';
import 'package:fixnum/fixnum.dart';

@Component(
    selector: "contact-list",
    templateUrl: "contact_list_component.html",
    styleUrls: ["contact_list_component.css"],
    directives: [coreDirectives, formDirectives],
    exports: [UserRelation])
class ContactListComponent implements OnActivate {
  List<UserPublicProfile> searchedProfiles = List<UserPublicProfile>();
  List<UserPublicProfile> inviterProfiles = List<UserPublicProfile>();
  List<UserPublicProfile> inviteeProfiles = List<UserPublicProfile>();
  List<UserPublicProfile> contactProfiles = List<UserPublicProfile>();

  static const int _kResultPerPage = 20;

  Pagination searchPagination = Pagination()..resultPerPage = _kResultPerPage;
  Pagination inviterPagination = Pagination()..resultPerPage = _kResultPerPage;
  Pagination inviteePagination = Pagination()..resultPerPage = _kResultPerPage;
  Pagination contactPagination = Pagination()..resultPerPage = _kResultPerPage;

  bool onLoadingSearchedProfiles = false;
  bool onLoadingInviterProfiles = false;
  bool onLoadingInviteeProfiles = false;
  bool onLoadingContactProfiles = false;

  final UserServiceInterface _user_service;
  final SocialNetworkServiceInterface _social_network_service;
  final Router _router;

  SearchResultSync<SearchUserResponse> _searchSync =
      SearchResultSync<SearchUserResponse>();

  ContactListComponent(
      this._user_service, this._social_network_service, this._router);

  @override
  void onActivate(_, RouterState current) async {
    onLoadingInviterProfiles = true;
    _social_network_service
        .getRelatedUserList(
            GetRelatedUserListRequest()
              ..pagination = inviterPagination
              ..relationFilter.add(UserRelation.URL_INVITATION_RECEIVED),
            credentialStorage.loadSignature())
        .then((GetRelatedUserListResponse res) {
      inviterProfiles = res.userProfiles;
      onLoadingInviterProfiles = false;
    });

    onLoadingInviteeProfiles = true;
    _social_network_service
        .getRelatedUserList(
            GetRelatedUserListRequest()
              ..pagination = inviterPagination
              ..relationFilter.add(UserRelation.URL_INVITATION_SENT),
            credentialStorage.loadSignature())
        .then((GetRelatedUserListResponse res) {
      inviteeProfiles = res.userProfiles;
      onLoadingInviteeProfiles = false;
    });

    onLoadingContactProfiles = true;
    _social_network_service
        .getRelatedUserList(
            GetRelatedUserListRequest()
              ..pagination = inviterPagination
              ..relationFilter.add(UserRelation.URL_CONTACT),
            credentialStorage.loadSignature())
        .then((GetRelatedUserListResponse res) {
      contactProfiles = res.userProfiles;
      onLoadingContactProfiles = false;
    });
  }

  void onKeyDownSearchContact(String searchInput) {
    SearchUserRequest req = SearchUserRequest();
    if (searchInput.isNotEmpty) {
      req.alias = (NullableString()..value = searchInput);
      try {
        Int64 userIdSearchPrefix = Int64.parseInt(searchInput);
        req.userId = (NullableInt64()..value = userIdSearchPrefix);
      } catch (err) {}
    }
    req.pagination = searchPagination;

    onLoadingSearchedProfiles = true;
    Future<SearchUserResponse> currentSearchFuture =
        _user_service.search(req, credentialStorage.loadSignature());
    _searchSync.takeLatestFuture(currentSearchFuture, (SearchUserResponse res) {
      searchedProfiles = res.userProfiles;
      onLoadingSearchedProfiles = false;
    });
  }

  String _accountDetailsUrl(Int64 userId) {
    return RoutePaths.account.toUrl(parameters: {kIdPathVariable: "$userId"});
  }

  void onSelectUserDetails(UserPublicProfile userProfile) {
    _router.navigate(_accountDetailsUrl(userProfile.userId));
  }

  UserRelationRecord extractRelation(
      List<UserRelationRecord> relations, UserRelation expected) {
    return relations.firstWhere(
        (UserRelationRecord relation) => relation.relation == expected);
  }

  String relationJoinDateString(UserRelationRecord relation) {
    return DateTime.fromMillisecondsSinceEpoch(
            relation.createdAt.toInt() * 1000)
        .toLocal()
        .toString();
  }

  bool unrelated(List<UserRelationRecord> relations) {
    return relations.isEmpty;
  }

  bool invitationPending(List<UserRelationRecord> relations) {
    return relations.any((UserRelationRecord relation) =>
        relation.relation == UserRelation.URL_INVITATION_SENT);
  }

  bool invitationReceived(List<UserRelationRecord> relations) {
    return relations.any((UserRelationRecord relation) =>
        relation.relation == UserRelation.URL_INVITATION_RECEIVED);
  }

  bool contact(List<UserRelationRecord> relations) {
    return relations.any((UserRelationRecord relation) =>
        relation.relation == UserRelation.URL_CONTACT);
  }

  bool blocked(List<UserRelationRecord> relations) {
    return relations.any((UserRelationRecord relation) =>
        relation.relation == UserRelation.URL_BLOCKED);
  }

  bool blocking(List<UserRelationRecord> relations) {
    return relations.any((UserRelationRecord relation) =>
        relation.relation == UserRelation.URL_BLOCKING);
  }
}
