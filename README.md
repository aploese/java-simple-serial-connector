# SPSW Serial Port Socket Wrapper

Access the serial device like UART, usb to serial converter or even a TCP bridge to an serial device on a different machine.
It implements ByteChannel and InterruptableChannel so any IO-operation can be interruped calling interrupt() on the thread
that does the I/O operations. This results then in an ClosedByInterruptException.
Any read operations are synchronized as well as all write operations.  
Closing an socket with pending IO operations will unblock said operations and throw an AsynchonousCloseException.

## Maven Dependencies

In your library add this dependency.
```
<dependency>
    <groupId>de.ibapl.spsw</groupId>
    <artifactId>de.ibapl.spsw.api</artifactId>
    <version>3.0.0-SNAPSHOT</version>
</dependency>
```

In the final application add one of this providers to the runtime only.
```
<dependency>
    <groupId>de.ibapl.spsw</groupId>
    <artifactId>de.ibapl.spsw.jnhwprovider</artifactId>
    <version>2.0.0-SNAPSHOT</version>
    <scope>runtime</scope>
</dependency>
```
or
```
<dependency>
    <groupId>de.ibapl.spsw</groupId>
    <artifactId>de.ibapl.spsw.jniprovider</artifactId>
    <version>2.0.0-SNAPSHOT</version>
    <scope>runtime</scope>
</dependency>
```

## Code Examples

### Load SerialPortSocketFactory

#### OSGi
Just use the OSGi annotation @Reference.
```java
@Reference
List<SerialPortSocketFactory> loader;
```

#### J2SE with java.util.ServiceLoader

Use the ServiceLoader to load all instances of SerialPortSocketFactory. Usually there should be only one - but prepared for the other.

```java
ServiceLoader<SerialPortSocketFactory> loader = ServiceLoader.load(SerialPortSocketFactory.class);
Iterator<SerialPortSocketFactory> iterator = loader.iterator();
if (!iterator.hasNext()) {
	LOG.severe("NO implementation of SerialPortSocketFactory available - add a provider for that to the test dependencies");
}
SerialPortSocketFactory serialPortSocketFactory = iterator.next();
if (iterator.hasNext()) {
	StringBuilder sb = new StringBuilder("More than one implementation of SerialPortSocketFactory available - fix the test dependencies\n");
	iterator = loader.iterator();
	while ( iterator.hasNext()) {
        sb.append(iterator.next().getClass().getCanonicalName()).append("\n");
	}
    LOG.severe(sb.toString());
}
```

### Open Try With Resource

```java
try (SerialPortSocket serialPortSocket = serialPortSocketFactory.open(PORT_NAME) {
	serialPortSocket.getOutputStream().write("Hello World!".getBytes());
} catch (IOException ioe) {
	System.err.println(ioe);
}
```

### Open Try With Resource And Parameters

```java
try (SerialPortSocket serialPortSocket = serialPortSocketFactory.open(PORT_NAME, Speed._9600_BPS, DataBits.DB_8, StopBits.SB_1, Parity.NONE, FlowControl.getFC_NONE()) {
	serialPortSocket.getOutputStream().write("Hello World!".getBytes());
} catch (IOException ioe) {
	System.err.println(ioe);
}
```

### Create and Open

```java
SerialPortSocket serialPortSocket = serialPortSocketFactory.open(PORT_NAME, Speed._9600_BPS, DataBits.DB_8, StopBits.SB_1, Parity.NONE, FlowControl.getFC_NONE());
try {
    serialPortSocket.getOutputStream().write("Hello World!".getBytes());
} catch (IOException ioe) {
    System.err.println(ioe);
} finally {
    serialPortSocket.close();
}
```

### Setting Timeouts
Set the `interByteReadTimeout` to 100 ms. The `interByteReadTimeout` is the time to wait after data has been received before return from wait.

Set the `overallReadTimeout` to 1000 ms. The `overallReadTimeout` is the time to wait if no data is received before return from wait.

Set the `overallWriteTimeout` to 2000 ms.

Be aware that the read amount of wait time is implementation dependant.

```java
try {
	serialPortSocket.setTimeouts(100, 1000, 2000);
	serialPortSocket.getOutputStream().write("Hello World!".getBytes());
	final byte[] buf = new byte["Hello World!".length()];
	final int len = serialPortSocket.getInputStream().read(buf);
	for (int i = 0; i < len; i++) {
		System.out.print((char)buf[i]);
	}
	System.out.println();
} catch (TimeoutIOException tioe) {
	System.err.println(tioe);
} catch (InterruptedIOException iioe) {
	System.err.println(iioe);
} catch (IOException ioe) {
	System.err.println(ioe);
} finally {
	serialPortSocket.close();
}

```

## Logging

To create an ascii logger do the following

```java
ServiceLoader<SerialPortSocketFactory> sl = ServiceLoader.load(SerialPortSocketFactory.class);
Iterator<SerialPortSocketFactory> i = sl.iterator();
if (!i.hasNext()) {
	throw new RuntimeException("No provider for SerialPortSocketFactory found, pleas add one to you class path ");
}
final SerialPortSocketFactory serialPortSocketFactory = i.next();
File logFile = File.createTempFile("Log_" + portName, ".txt");
SerialPortSocket serialPort = LoggingSerialPortSocket.wrapWithAsciiOutputStream(serialPortSocketFactory.createSerialPortSocket(portName), new FileOutputStream(logFile), false, TimeStampLogging.NONE);

```

If you want the bytes passed formatted as hex use `LoggingSerialPortSocket.wrapWithHexOutputStream` instead.

## Ser2Net

Use `Ser2NetProvider(host, dataPort)` to get an instance of `SerialPortSocket`.


## Testing Hardware

1.  Grap yourself 2 serial devices and a null modem adapter or a null modem cable. OR use one device with properly cross connected lines [Null modem](https://www.wikipedia.org/wiki/Null_modem) .
1.  Goto the directory `de.ibapl.spsw.jniprovider`.
2.  copy `src/test/resources/junit-spsw-config.properties.template` to `src/test/resources/junit-spsw-config.properties`.
3.  Edit the portnames in `src/test/resources/junit-spsw-config.properties`. If you have a "cross connected" device use the same name for readPort a writePort.
4.  Run `mvn -PBaselineTests test` for the Baseline tests. This tests should never fail.
5.  `mvn  test` to execute all tests - some will fail. Look at the test itself and on the outcome to device whats up.

## Demos

First go to the [subdirectory it](./it) and execute `mvn package` to build all demos, but not install them.

### Print ports to standard out

Go to the subdirectory it/print-ports and execute
`mvn exec:java -Dexec.mainClass="de.ibapl.spsw.demo.print.PrintPortsDemoMain"`
to list all ports and their state.


