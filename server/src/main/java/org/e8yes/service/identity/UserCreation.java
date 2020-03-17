package org.e8yes.service.identity;

import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.ibatis.session.SqlSession;
import org.e8yes.connection.DatabaseConnection;
import org.e8yes.exception.ResourceConflictException;
import org.e8yes.service.EtUser;
import org.e8yes.service.EtUserGroup;
import org.e8yes.service.identity.ctx.AUserGroupContext;
import org.e8yes.service.identity.dao.mappers.AUserMapper;
import org.e8yes.util.Time;
import org.mindrot.jbcrypt.BCrypt;

/**
 * Business logic for user management.
 *
 * @author davis
 */
public class User {

  public static void init(AUserGroupContext userGroupCtx) {
    try {
      // Create root user.
      createUser(
          "root",
          "superuser",
          "c6cefcab-67fc-414d-b105-7ce7f0faae9f",
          userGroupCtx.getSystemUserGroup(AUserGroupContext.SystemUserGroup.SUPER_USER_GROUP));
    } catch (ResourceConflictException ex) {
      Logger.getLogger(User.class.getName()).log(Level.INFO, null, "root user exists.");
      Logger.getLogger(User.class.getName()).log(Level.INFO, null, ex);
    }
  }

  private static EtUser createUser(
      String userName, String alias, String passWord, EtUserGroup userGroup)
      throws ResourceConflictException {
    SqlSession sess = DatabaseConnection.openSession();
    AUserMapper mapper = sess.getMapper(AUserMapper.class);

    EtUser user =
        EtUser.newBuilder()
            .setId(mapper.reservePk())
            .setUserName(userName)
            .setAlias(alias)
            .setPasscode(BCrypt.hashpw(passWord, BCrypt.gensalt()))
            .setCreatedAtSec(Time.curTimestampSec())
            .setStatus(EtUser.UserStatus.ACTIVE)
            .setGroupId(userGroup.getId())
            .build();

    int rowsAffected = mapper.save(user);
    if (rowsAffected != 1) {
      throw new ResourceConflictException();
    }
    sess.commit();
    DatabaseConnection.closeSession(sess);
    return user;
  }

  public static EtUser createBaselineUser(
      String userName, String alias, String passWord, AUserGroupContext userGroupCtx)
      throws ResourceConflictException {
    return createUser(
        userName,
        alias,
        passWord,
        userGroupCtx.getSystemUserGroup(AUserGroupContext.SystemUserGroup.BASELINE_USER_GROUP));
  }

  public static boolean userNameExists(String userName) {
    SqlSession sess = DatabaseConnection.openSession();
    AUserMapper mapper = sess.getMapper(AUserMapper.class);
    boolean existence = mapper.existsByIdOrUserName(null, userName);
    DatabaseConnection.closeSession(sess);
    return existence;
  }
}