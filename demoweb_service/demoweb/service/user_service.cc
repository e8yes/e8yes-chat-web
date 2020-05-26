/**
 * e8yes demo web server.
 *
 * <p>Copyright (C) 2020 Chifeng Wen {daviesx66@gmail.com}
 *
 * <p>This program is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * <p>You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <grpcpp/grpcpp.h>
#include <optional>
#include <string>

#include "demoweb_service/demoweb/common_entity/user_entity.h"
#include "demoweb_service/demoweb/environment/environment_context_interface.h"
#include "demoweb_service/demoweb/module_identity/create_user.h"
#include "demoweb_service/demoweb/module_identity/retrieve_user.h"
#include "demoweb_service/demoweb/module_identity/user_identity.h"
#include "demoweb_service/demoweb/service/user_service.h"

namespace e8 {

grpc::Status UserServiceImpl::Register(grpc::ServerContext * /*context*/,
                                       RegistrationRequest const *request,
                                       RegistrationReponse *response) {

    std::optional<UserEntity> user =
        CreateBaselineUser(request->security_key(),
                           /*userId=*/std::nullopt, CurrentEnvironment()->CurrentHostId(),
                           CurrentEnvironment()->DemowebDatabase());
    if (!user.has_value()) {
        return grpc::Status(grpc::StatusCode::INTERNAL,
                            "User ID conflicts when it shouldn't happen");
    }

    response->set_user_id(user.value().id.value().value());
    response->set_error_type(RegistrationReponse::RET_NoError);

    return grpc::Status::OK;
}

grpc::Status UserServiceImpl::Authorize(grpc::ServerContext * /*context*/,
                                        AuthorizationRequest const *request,
                                        AuthorizationResponse *response) {
    std::optional<UserEntity> user =
        RetrieveUser(request->user_id(), CurrentEnvironment()->DemowebDatabase());
    if (!user.has_value()) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND,
                            "User ID=" + std::to_string(request->user_id()) + " doesn't exist.");
    }

    std::optional<SignedIdentity> signed_identity =
        SignIdentity(user.value(), request->security_key(), CurrentEnvironment()->KeyGen());
    if (!signed_identity.has_value()) {
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED,
                            "Failed to validate the provided security key.");
    }

    response->set_signed_identity(signed_identity.value().data(), signed_identity.value().size());

    return grpc::Status::OK;
}

} // namespace e8
