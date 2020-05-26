QT       -= core gui
TARGET = demowebservice
TEMPLATE = lib
CONFIG += c++17

QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -flto
QMAKE_LDFLAGS_RELEASE += -O3 -flto

DEFINES += DEMOWEBLIB_LIBRARY
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../../

HEADERS += \
    common_entity/file_metadata_entity.h \
    common_entity/user_entity.h \
    common_entity/user_group_entity.h \
    common_entity/user_group_has_file_entity.h \
    constant/context_key.h \
    module_identity/create_user.h \
    module_identity/retrieve_user.h \
    module_identity/user_identity.h \
    module_identity/user_profile.h \
    module_file/file_io.h \
    module_file/file_metadata.h \
    module_rbac/file_access_validator.h \
    module_rbac/system_user_group.h \
    environment/environment_context_interface.h \
    environment/test_environment_context.h \
    constant/demoweb_database.h \
    environment/host_id.h \
    environment/prod_environment_context.h \
    proto_cc/encryption_source.grpc.pb.h \
    proto_cc/encryption_source.pb.h \
    proto_cc/file.grpc.pb.h \
    proto_cc/file.pb.h \
    proto_cc/identity.grpc.pb.h \
    proto_cc/identity.pb.h \
    proto_cc/invitation_status.grpc.pb.h \
    proto_cc/invitation_status.pb.h \
    proto_cc/nullable_primitives.grpc.pb.h \
    proto_cc/nullable_primitives.pb.h \
    proto_cc/pagination.grpc.pb.h \
    proto_cc/pagination.pb.h \
    proto_cc/permission.grpc.pb.h \
    proto_cc/permission.pb.h \
    proto_cc/service_file.grpc.pb.h \
    proto_cc/service_file.pb.h \
    proto_cc/service_socialnetwork.grpc.pb.h \
    proto_cc/service_socialnetwork.pb.h \
    proto_cc/service_system.grpc.pb.h \
    proto_cc/service_system.pb.h \
    proto_cc/service_user.grpc.pb.h \
    proto_cc/service_user.pb.h \
    proto_cc/user_profile.grpc.pb.h \
    proto_cc/user_profile.pb.h \
    service/file_service.h \
    service/user_service.h
SOURCES += \
    common_entity/file_metadata_entity.cc \
    common_entity/user_entity.cc \
    common_entity/user_group_entity.cc \
    common_entity/user_group_has_file_entity.cc \
    module_identity/create_user.cc \
    module_identity/retrieve_user.cc \
    environment/environment_context_interface.cc \
    environment/test_environment_context.cc \
    environment/host_id.cc \
    environment/prod_environment_context.cc \
    module_identity/user_identity.cc \
    module_identity/user_profile.cc \
    module_file/file_io.cc \
    module_file/file_metadata.cc \
    module_rbac/file_access_validator.cc \
    proto_cc/encryption_source.grpc.pb.cc \
    proto_cc/encryption_source.pb.cc \
    proto_cc/file.grpc.pb.cc \
    proto_cc/file.pb.cc \
    proto_cc/identity.grpc.pb.cc \
    proto_cc/identity.pb.cc \
    proto_cc/invitation_status.grpc.pb.cc \
    proto_cc/invitation_status.pb.cc \
    proto_cc/nullable_primitives.grpc.pb.cc \
    proto_cc/nullable_primitives.pb.cc \
    proto_cc/pagination.grpc.pb.cc \
    proto_cc/pagination.pb.cc \
    proto_cc/permission.grpc.pb.cc \
    proto_cc/permission.pb.cc \
    proto_cc/service_file.grpc.pb.cc \
    proto_cc/service_file.pb.cc \
    proto_cc/service_socialnetwork.grpc.pb.cc \
    proto_cc/service_socialnetwork.pb.cc \
    proto_cc/service_system.grpc.pb.cc \
    proto_cc/service_system.pb.cc \
    proto_cc/service_user.grpc.pb.cc \
    proto_cc/service_user.pb.cc \
    proto_cc/user_profile.grpc.pb.cc \
    proto_cc/user_profile.pb.cc \
    service/file_service.cc \
    service/user_service.cc

unix:!macx: LIBS += -L$$OUT_PWD/../../postgres/query_runner/ -lquery_runner

INCLUDEPATH += $$PWD/../../postgres/query_runner
DEPENDPATH += $$PWD/../../postgres/query_runner

unix:!macx: LIBS += -L$$OUT_PWD/../../keygen/ -lkeygen

INCLUDEPATH += $$PWD/../../keygen
DEPENDPATH += $$PWD/../../keygen

LIBS += -lcrypt
LIBS += -lcrypto++
LIBS += -lpqxx
LIBS += -pthread
LIBS += -ldl
LIBS += -lprotobuf
LIBS += -lgrpc++
LIBS += -lgrpc++_reflection
