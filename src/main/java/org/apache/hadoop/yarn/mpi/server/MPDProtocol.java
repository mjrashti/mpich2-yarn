/**
 *
 */
package org.apache.hadoop.yarn.mpi.server;

import org.apache.hadoop.ipc.VersionedProtocol;

/**
 * An MPDProtocol is a protocol that <code>Container</code> communicates to
 * <code>ApplicationMaster</code>
 */
public interface MPDProtocol extends VersionedProtocol {

  /**
   * Version Id
   */
  public static final long versionID = 1L;

  /**
   * RPC Method, report the container's status
   *
   * @param containerStatus
   *          The current container's status
   */
  /*MJR added the exp argument*/
  void reportStatus(ContainerId containerId, MPDStatus containerStatus,String expMsg);
  void reportStatus(ContainerId containerId, MPDStatus containerStatus);

  /**
   * RPC Method, container ping the applicationMasater in order to tell that it
   * is alive and receive whether the container should finish.
   *
   * @param containerId
   * @return Whether the container should finish
   */
  boolean ping(ContainerId containerId);
}
