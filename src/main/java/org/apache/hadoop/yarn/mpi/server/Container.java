package org.apache.hadoop.yarn.mpi.server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.net.InetSocketAddress;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

/*MJR added*/
import java.io.ByteArrayOutputStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.concurrent.LinkedBlockingQueue;
import org.apache.hadoop.mapred.JobConf;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FileUtil;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.ipc.RPC;
import org.apache.hadoop.yarn.api.ApplicationConstants;
import org.apache.hadoop.yarn.mpi.MPIConfiguration;
import org.apache.hadoop.yarn.mpi.MPIConstants;
import org.apache.hadoop.yarn.mpi.util.FileSplit;
import org.apache.hadoop.yarn.mpi.util.LocalFileUtils;
import org.apache.hadoop.yarn.mpi.util.MPDException;
import org.apache.hadoop.yarn.mpi.util.MPIResult;
import org.apache.hadoop.yarn.mpi.util.Utilities;
import org.apache.hadoop.yarn.util.ConverterUtils;

import com.google.common.util.concurrent.ThreadFactoryBuilder;

public class Container {

  private static final Log LOG = LogFactory.getLog(Container.class);
  private TaskReporter taskReporter;

  private final ExecutorService executorDownload;

  private static final int POOL_SIZE = 4;

  private String localDir;

  private final Configuration conf;
  private MPDProtocol protocol;
  private String appMasterHost;
  private int appMasterPort;
  private ContainerId containerId = null;
  private final String sshAuthorizedKeysPath;

  private String appAttemptID;

   //MJR added
  // A queue that keep track of the mpi messages
  private final LinkedBlockingQueue<String> mpiMsgs = new LinkedBlockingQueue<String>(
      MPIConstants.MAX_LINE_LOGS);
  private String mpiNameService = null;
  private int numMpiProcs = 1;

  private Boolean downloadSave = false;

  private Collection<MPIResult> results;
  private boolean cleanedSuccessfully = false;

  public Container() {
    conf = new MPIConfiguration();
    executorDownload = Executors.newFixedThreadPool(
        POOL_SIZE,
        new ThreadFactoryBuilder().setDaemon(true)
        .setNameFormat("Download Thread #%d").build());
    sshAuthorizedKeysPath = conf.get(MPIConfiguration.MPI_SSH_AUTHORIZED_KEYS_PATH);
  }

  public boolean init(String[] args) throws ParseException, IOException {
    Options options = new Options();

    //MJR added
    numMpiProcs = Integer.parseInt(args[0]);
    mpiNameService = args[1].substring(args[1].indexOf("=") + 1,args[1].length());
    //Log.info("mpiNameService = "+mpiNameService);

    containerId = new ContainerId(ConverterUtils.toContainerId(System
        .getenv(ApplicationConstants.Environment.CONTAINER_ID.toString())));
    if (containerId == null) {
      LOG.error("No container ID in env.");
      throw new ParseException("Container Id is not defined");
    }

    appMasterHost = System.getenv("APPMASTER_HOST");
    appMasterPort = Integer.valueOf(System.getenv("APPMASTER_PORT"));
    InetSocketAddress addr = new InetSocketAddress(appMasterHost, appMasterPort);
    protocol = RPC.getProxy(MPDProtocol.class, MPDProtocol.versionID, addr,
        conf);
    protocol.reportStatus(containerId, MPDStatus.INITIALIZED);
    taskReporter = new TaskReporter(protocol, conf, containerId);
    taskReporter.setDaemon(true);
    taskReporter.start();

    Map<String, String> envs = System.getenv();
    localDir = Utilities.getDownLoadDir(conf,
        envs.get(MPIConstants.APPATTEMPTID), containerId);
    LOG.info(String.format("localDir:%s", localDir));
    appAttemptID = envs.get(MPIConstants.APPATTEMPTID);
    downloadSave = conf.getBoolean(
        MPIConfiguration.MPI_CONTAINER_DOWNLOAD_SAVE, false);
    String containerOutput = envs.get(MPIConstants.CONTAINOUTPUT);
    results = Utilities.decodeMPIResult(containerOutput);
    return true;
  }

  /**
   * download files if necessary
   *
   * @throws IOException
   * @throws ExecutionException
   */
  public Boolean download() throws IOException, InterruptedException,
  ExecutionException {
    Map<String, String> envs = System.getenv();
    String fileSplits = envs.get(MPIConstants.CONTAININPUT);
    List<FileSplit> splits = Utilities.decodeSplt(fileSplits,
        FileSystem.get(conf));
    List<ContainerDownLoad> downLoads = new ArrayList<ContainerDownLoad>();
    boolean allDownLoadSuccess = true;
    boolean mergeSuccess = true;

    if (splits != null && splits.size() > 0) {
      File dirExist = new File(localDir);
      if (dirExist.exists()) {
        dirExist.delete();
      }
      LocalFileUtils.mkdirs(localDir);
      List<String> downloadResult = new ArrayList<String>();
      for (FileSplit fSplit : splits) {
        downloadResult.clear();
        downLoads.clear();
        LOG.info(String.format("begin to download the following files:%s",
            fSplit.getSplits()));
        int i = 1;
        for (Path path : fSplit.getSplits()) {
          String downLoadOut = localDir + System.currentTimeMillis() + "-"
              + i++;
          ContainerDownLoad download = new ContainerDownLoad(path,
              FileSystem.get(conf), downLoadOut, conf);
          downLoads.add(download);
        }
        LOG.info(String.format("download size: %d", downLoads.size()));
        List<Future<String>> results = this.executorDownload
            .invokeAll(downLoads);

        LOG.info(String.format("result size: %d", results.size()));

        for (Future<String> result : results) {
          if (result.get() == null) {
            allDownLoadSuccess = false;
          } else {
            downloadResult.add(result.get());
          }
        }
        if (allDownLoadSuccess) {
          LOG.info(String.format(
              "download the following files:%s successfully",
              fSplit.getSplits()));
          LOG.info(String
              .format(
                  "begin to merge the following  files:%s,and the merge file name:%s",
                  downloadResult, fSplit.getDownFileName()));
          ContainerMerge merge = new ContainerMerge(downloadResult,
              fSplit.getDownFileName());
          FutureTask<Boolean> mergeTask = new FutureTask<Boolean>(merge);
          Thread mergeThread = new Thread(mergeTask);
          mergeThread.start();
          mergeSuccess = mergeTask.get();
          if (mergeSuccess) {
            LOG.info(String.format(
                "merge the following  files:%s successfully", downloadResult));
          } else {
            LOG.error(String.format("fail to merge the following  files:%s",
                downloadResult));
          }
        } else {
          LOG.info(String.format("fail to download the following files:%s ",
              fileSplits));
        }

        if (!allDownLoadSuccess || !mergeSuccess) {
          break;
        }
      }
    }
    if (allDownLoadSuccess && mergeSuccess) {
      return true;
    } else {
      return false;
    }
  }

  // upload file from container to hdfs
  public boolean upload() throws IOException {
    if (results != null && results.size() > 0) {
      Iterator<MPIResult> itResult = results.iterator();
      while (itResult.hasNext()) {
        MPIResult mr = itResult.next();
        FileSystem localFs = FileSystem.getLocal(conf);
        FileSystem dfs = FileSystem.get(conf);
        Path localPath = new Path(mr.getContainerLocal());
        Path resultPath = new Path(mr.getDfsLocation());
        if (localFs.exists(localPath)) {
          if (!dfs.isDirectory(resultPath) && dfs.exists(resultPath)) {
            throw new MPDException(String.format("file %s exists",
                resultPath.toString()));
          } else {
            dfs.copyFromLocalFile(false, false, localPath, resultPath);
          }
        }
      }
    }

    return true;
  }

  /**
   * Copy necessary files needed for the MPI program
   */
  public void copyMPIExecutable() {
    Map<String, String> envs = System.getenv();
    String mpiExecDir = envs.get("MPIEXECDIR");
    LocalFileUtils.mkdirs(mpiExecDir);
    File mpiexecCwd = new File("./MPIExec");
    File mpiexecSame = new File(mpiExecDir + "/MPIExec");
    LocalFileUtils.copyFile(mpiexecCwd, mpiexecSame);
    mpiexecSame.setExecutable(true);
  }

  /*MJR added these following function*/ 

  public void appendMsg(String message) {
    Boolean flag = mpiMsgs.offer(message);
    if (!flag) {
      LOG.warn("message queue is full");
    }
  }

  //MJR added
  private StringBuilder buildMpiCommand(String mpiImpl){
    StringBuilder commandBuilder;
    if(mpiImpl.equals("OPENMPI")){
        commandBuilder = new StringBuilder("mpiexec -ompi-server \"");
	commandBuilder.append(mpiNameService);
	try{
        	commandBuilder.append("\" -host "+InetAddress.getLocalHost().getHostName()+" -n "+numMpiProcs+" ");
	}catch(UnknownHostException e){
		e.printStackTrace();
		return null;
	}
    }else if(mpiImpl.equals("MPICH")){
        commandBuilder = new StringBuilder("mpiexec -launcher ssh -nameserver ");
	commandBuilder.append(mpiNameService);
	try{
		commandBuilder.append(" -hosts "+InetAddress.getLocalHost().getHostName()+" -np  "+numMpiProcs+" ");
	}catch(UnknownHostException e){
                e.printStackTrace();
                return null;
        }
    }
    else return null;
    return commandBuilder;	
  }

  private boolean launchMpiClient() throws IOException {
    LOG.info("Launching mpi client process from the Container...");
    this.appendMsg("Launching mpi client process from the Container...");
    Map<String, String> env = System.getenv();
    LOG.info("got env");
    StringBuilder commandBuilder = buildMpiCommand(MPIConstants.YARN_MPI_IMPL);
    if(commandBuilder == null){
	return true;
    }
    commandBuilder.append(" ");

/*
    //String wrapperPath = env.get(MPIConstants.AMJARLOCATION);
    //String wrapperPath = JobConf.findContainingJar(ApplicationMaster.class);
	//FIXME NOW
    String wrapperPath = "/data/home/mrashti/projects/informer_hpcc/mpich2-yarn/target/mpich2-yarn-1.0-SNAPSHOT.jar";


    int idx = wrapperPath.indexOf(MPIConstants.TARGETJARNAME);


    commandBuilder.append(wrapperPath.substring(0,idx-1));

    commandBuilder.append("/test_mpi_connect_client ");
*/

    commandBuilder.append(env.get("MPIEXECDIR"));
    commandBuilder.append("/MPIExec");


    String[] envs = new String[1];
    envs[0] = "PATH="+System.getenv("PATH");			
    /*FIXME: How to get this keypair_position here?*/
    /*if(MPIConstants.YARN_MPI_IMPL.equals("MPICH"))
	envs[1] = "HYDRA_LAUNCHER_EXTRA_ARGS=-o StrictHostKeyChecking=no -i " + keypair_position;*/

    LOG.info("Executing command:" + commandBuilder.toString());
    String execDir = env.get("MPIEXECDIR");//wrapperPath.substring(0,idx-1);
    File execPWD = new File(execDir);
    Runtime rt = Runtime.getRuntime();
 
    LOG.info("Running "+commandBuilder.toString()+ " on "+InetAddress.getLocalHost().getHostName());

    final Process pc = rt.exec(commandBuilder.toString(), envs, execPWD);

    Thread stdinThread = new Thread(new Runnable() {
      @Override
      public void run() {
        Scanner pcStdout = new Scanner(pc.getInputStream());
        while (pcStdout.hasNextLine()) {
          String line = "[stdout] " + pcStdout.nextLine();
          LOG.info(line);
	  appendMsg(line);
        }
        pcStdout.close();
      }
    });
    stdinThread.start();

    Thread stderrThread = new Thread(new Runnable() {
      @Override
      public void run() {
        Scanner pcStderr = new Scanner(pc.getErrorStream());
        while (pcStderr.hasNextLine()) {
          String line = "[stderr] " + pcStderr.nextLine();
          LOG.info(line);
	  appendMsg(line);
        }
        pcStderr.close();
      }
    });
    stderrThread.start();

    try {
      int ret = pc.waitFor();

      if (ret != 0) {
        return false;
      } else {
        return true;
      }
    } catch (InterruptedException e) {
      LOG.error("mpiexec Thread is nterruptted!", e);
    }

  
    return false;
  }


  public Boolean run() throws IOException {
    // TODO Is there any outputs of daemons such as ssh?

    final String publicKey = System.getenv(MPIConstants.AM_PUBLIC_KEY);
    if (publicKey == null || publicKey.isEmpty()) {
      LOG.error("Public key isn't distributed to container, fail!");
      protocol.reportStatus(containerId, MPDStatus.MPD_CRASH);
      return false;
    }

    cleanedSuccessfully = false;

    allowPublicKey(publicKey);

    protocol.reportStatus(containerId, MPDStatus.MPD_STARTED);

    int taskPingInterval = this.conf.getInt(
        MPIConfiguration.TASK_PING_INTERVAL, 1000);
    int taskPingRetry = this.conf.getInt(MPIConfiguration.TASK_PING_RETRY, 3);

    Thread keyCleanupHook = new Thread("key_cleanup_shutdown_hook") {
      @Override
      public void run() {
        if (!cleanedSuccessfully) {
          try {
            LOG.info("Container quit but need clean up, doing the clean-up job.");
            disallowPublicKey(publicKey);
            LOG.info("Clean up done.");
          } catch (FileNotFoundException e) {
            e.printStackTrace();
          } catch (IOException e) {
            e.printStackTrace();
          }
          super.run();
        }
      }
    };
    Runtime.getRuntime().addShutdownHook(keyCleanupHook);

    //MJR added to run the mpi program from container//
    LOG.info("mpiNameService: "+mpiNameService);
    if(mpiNameService != null)
	launchMpiClient(); 
	

    LOG.info("Wait for the AM's signal.");

    // Wait until AM tells that the task has been finished
    while (true) {
      boolean exit = false;
      for (int i = 0; i < taskPingRetry; i++) {
        try {
          exit = protocol.ping(containerId);
          break;
        } catch (Exception e) {
          LOG.error("Communication exception:", e);
        }
      }
      if (exit)
        break;
      try {
        Thread.sleep(taskPingInterval);
      } catch (InterruptedException e) {
        LOG.info("Container main thread interrupted");
        break;
      }
    }

    // Now disallow the public key to login again.
    disallowPublicKey(publicKey);
    cleanedSuccessfully = true;

    Boolean runSuccess = true;
    protocol.reportStatus(containerId, MPDStatus.FINISHED);
    LOG.info(String.format("Container %s, smpd finish successfully",
        containerId.toString()));

    return runSuccess;
  }

  /**
   * @param publicKey
   * @throws IOException
   * @throws FileNotFoundException
   */
  private void allowPublicKey(String publicKey) throws IOException,
  FileNotFoundException {
    LOG.info("enable the public key: " + publicKey);
    // Enable the key-pair temporarily.
    File sshAuthorizedKeys = new File(sshAuthorizedKeysPath);
    if (!sshAuthorizedKeys.exists()) {
      LOG.info(sshAuthorizedKeys.getAbsolutePath()
          + " doesn't exist, creating it.");
      throw new IOException("Error - Does not exist: "+sshAuthorizedKeysPath+" user: "+System.getProperty("user.name"));	
      //sshAuthorizedKeys.createNewFile();
    }
    RandomAccessFile sshAuthorizedKeysOut = new RandomAccessFile(
        sshAuthorizedKeys, "rw");
    FileChannel sshAuthorizedKeysChannel = sshAuthorizedKeysOut.getChannel();

    @SuppressWarnings("unused")
    FileLock shareLock = sshAuthorizedKeysChannel.lock();

    long length = sshAuthorizedKeysOut.length();
    sshAuthorizedKeysOut.seek(length);
    sshAuthorizedKeysOut.writeBytes("\n" + publicKey);
    sshAuthorizedKeysOut.close();
  }

  /**
   * @param publicKey
   * @throws IOException
   * @throws FileNotFoundException
   */
  private void disallowPublicKey(String publicKey) throws IOException,
  FileNotFoundException {
    LOG.info("disable the public key: " + publicKey);
    File sshAuthorizedKeys = new File(sshAuthorizedKeysPath);
    if (!sshAuthorizedKeys.exists()) {
      LOG.info(sshAuthorizedKeys.getAbsolutePath()
          + " doesn't exist, strange problem.");
      return;
    }
    RandomAccessFile sshAuthorizedKeysOut = new RandomAccessFile(
        sshAuthorizedKeys, "rw");
    FileChannel sshAuthorizedKeysChannel = sshAuthorizedKeysOut.getChannel();

    @SuppressWarnings("unused")
    FileLock shareLock = sshAuthorizedKeysChannel.lock();

    LOG.info("lock acquired successfully");
    ArrayList<String> lines = new ArrayList<String>();
    FileReader reader = new FileReader(sshAuthorizedKeysOut.getFD());
    BufferedReader bufferedReader = new BufferedReader(reader);

    String line = bufferedReader.readLine();
    while (line != null) {
      LOG.info("Encountered: '" + line + "'");
      LOG.info("Publickeyis: '" + publicKey + "'");
      LOG.info("Equals: " + line.equals(publicKey));
      if (!line.equals(publicKey)) {
        lines.add(line);
      }
      line = bufferedReader.readLine();
    }
    LOG.info("authorized_keys read successfully with " + lines.size()
        + " entities.");

    sshAuthorizedKeysOut.seek(0);
    sshAuthorizedKeysOut.setLength(0);
    FileWriter writer = new FileWriter(sshAuthorizedKeysOut.getFD());
    BufferedWriter bufferedWriter = new BufferedWriter(writer);
    boolean first = true;
    for (String lineToWrite : lines) {
      if (first) {
        first = false;
        bufferedWriter.write(lineToWrite);
      } else {
        bufferedWriter.write("\n" + lineToWrite);
      }
    }
    bufferedWriter.flush();
    bufferedWriter.close();
    LOG.info("successfully disable the public key.");
  }

  public String getLocalDir() {
    return localDir;
  }

  /**
   * @param args
   * @throws IOException
   * @throws ParseException
   * @throws InterruptedException
   * @throws ExecutionException
   */
  public static void main(String[] args) {
    try {
      printDebugInfo();
    } catch (Exception e) {
      LOG.error("Error print debug info.");
      e.printStackTrace();
    }

    final Container container = new Container();
    Utilities.printRelevantParams("Container", container.conf);
    try {
      if (container.init(args)) {
        // add the shutdownHood after init
        if (!container.getDownloadSave()) {
          Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
              String deleteDir = Utilities.getApplicationDir(
                  container.getConf(), container.getAppAttemptID());
              try {
                FileUtil.fullyDelete(new File(deleteDir));
                LOG.info(String.format("clean the folder:%s successfully",
                    deleteDir));
              } catch (Exception e2) {
                LOG.error(String.format(
                    "error happens when cleaning the folder: %s", deleteDir),
                    e2);
              }
            }
          });
        }

        if (container.download()) {
          LOG.info("download successfully");
          container.copyMPIExecutable();
          LOG.info("copy mpi program successfully");
          Boolean runSuccess = container.run();
          if (runSuccess) {
            container.upload();
          }
        } else {
          container.getProtocol().reportStatus(container.getContainerId(),
              MPDStatus.ERROR_FINISHED1);
          LOG.error("downlaod failed!");
          System.exit(-1);
        }
      } else {
        container.getProtocol().reportStatus(container.getContainerId(),
            MPDStatus.ERROR_FINISHED2);
        LOG.error("Container init failed!");
        System.exit(-1);
      }
    } catch (Exception e) {
      LOG.error("Error executing MPI task in container.");
      e.printStackTrace();
      ByteArrayOutputStream baos = new ByteArrayOutputStream(1000);
      PrintWriter pr = new PrintWriter(baos,true);	
      e.printStackTrace(pr);
      pr.flush();	
      container.getProtocol().reportStatus(container.getContainerId(),
          MPDStatus.ERROR_FINISHED3,/*e.getMessage()*/ new String(baos.toByteArray()));
    }
  }

  /**
   * Print the environment and working directory information for debugging.
   *
   * @throws IOException
   */
  private static void printDebugInfo() throws IOException {
    File directory = new File(".");
    System.err.println(directory.getCanonicalPath());
    File mpiexec = new File("./MPIExec");
    System.err.println(mpiexec.getCanonicalPath());

    Map<String, String> envs = System.getenv();
    Set<Entry<String, String>> entries = envs.entrySet();
    for (Entry<String, String> entry : entries) {
      System.err.println("key=" + entry.getKey() + "; value="
          + entry.getValue());
    }
  }

  public Configuration getConf() {
    return conf;
  }

  public String getAppAttemptID() {
    return appAttemptID;
  }

  public Boolean getDownloadSave() {
    // TODO
    // return downloadSave;
    return true;
  }

  public MPDProtocol getProtocol() {
    return protocol;
  }

  public ContainerId getContainerId() {
    return containerId;
  }
}
