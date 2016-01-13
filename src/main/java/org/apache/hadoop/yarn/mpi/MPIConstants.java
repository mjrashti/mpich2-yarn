package org.apache.hadoop.yarn.mpi;

/**
 * Constants used in both Client and Application Master
 */
public class MPIConstants {

  
  /** MJR added
   * Environment key name pointing to the mpi's temp hdfs location (folder)
   */
  public static final String MPITEMPLOCATION = "MPITEMPLOCATION";
  //public static final String YARN_MPI_IMPL      =       "OPENMPI";
  public static final String YARN_MPI_IMPL      =       "MPICH";

  public static final boolean YARN_USE_MPI_SERVICE = false;
  /*END MJR*/

  public static final String AMJARLOCATION = "AMJARLOCATION";
  public static final String TARGETJARNAME = "mpich2-yarn-1.0-SNAPSHOT.jar";/*FIXME: This needs to be updated with version!*/

  /**
   * Environment key name pointing to the mpi program's hdfs location
   */
  public static final String MPIEXECLOCATION = "MPIEXECLOCATION";

  /**
   * Environment key name denoting the file timestamp for the mpi program.
   * Used to validate the local resource.
   */
  public static final String MPIEXECTIMESTAMP = "MPIEXECTIMESTAMP";

  /**
   * Environment key name denoting the file content length for the mpi program.
   * Used to validate the local resource.
   */
  public static final String MPIEXECLEN = "MPIEXECLEN";

  /**
   * Environment key name denoting MPI options
   */
  public static final String MPIOPTIONS = "MPIOPTIONS";

  /**
   * Environment key name pointing to the hdfs location of appmaster.jar
   */
  public static final String APPJARLOCATION = "APPJARLOCATION";

  /**
   * Environment key name denoting the file timestamp for the appmaster.jar
   * Used to validate the local resource.
   */
  public static final String APPJARTIMESTAMP = "APPJARTIMESTAMP";

  /**
   * Environment key name denoting the file content length for the appmaster.jar
   * Used to validate the local resource.
   */
  public static final String APPJARLEN = "APPJARLEN";

  /**
   * Environment key name pointing to the container's data location
   */
  public static final String CONTAININPUT = "CONTAININPUT";

  public static final String CONTAINOUTPUT = "CONTAINOUTPUT";

  /**
   * Environment key name pointing to the appAttemptID
   */
  public static final String APPATTEMPTID = "APPATTEMPTID";

  /**
   * the prefix of the input file
   */
  public static final String MPIINPUTS = "MPIINPUTS";

  /**
   * the prefix of the mpi result
   */
  public static final String MPIOUTPUTS = "MPIOUTPUTS";

  /**
   * process per container
   */
  public static final String PROCESSESPERCONTAINER = "PROCESSESPERCONTAINER";

  public static final int MAX_LINE_LOGS = 200;

  public static final String ALLOCATOR = "ALLOCATOR";

  public static final String AM_PUBLIC_KEY = "APPMASTER_PUBLIC_KEY";
}
