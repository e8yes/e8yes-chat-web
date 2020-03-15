package org.e8yes.exception;

/**
 * AccessDeniedException
 *
 * @author davis
 */
public class AccessDeniedException extends RpcException {

  @Override
  public String getMessage() {
    return "Access Denied";
  }

  @Override
  public int getStatusCode() {
    return 401;
  }
}
