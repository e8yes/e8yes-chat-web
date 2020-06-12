import 'package:demoweb_app/src/connection_manager_interface.dart';
import 'package:demoweb_app/src/identity_storage.dart';
import 'package:demoweb_app/src/immediate_connection.dart';

Uri demowebServiceUri = Uri.parse("http://api.e8yes.org:8000");

ConnectionManagerInterface demowebServiceConnections =
    ImmediateConnection(demowebServiceUri);

IdentityStorage identityStorage = IdentityStorage();

const int kSecurityKeyLength = 128;