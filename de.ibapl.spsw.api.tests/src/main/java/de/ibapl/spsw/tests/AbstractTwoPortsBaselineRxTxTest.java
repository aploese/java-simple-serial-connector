/*
 * SPSW - Drivers for the serial port, https://github.com/aploese/spsw/
 * Copyright (C) 2009-2021, Arne Plöse and individual contributors as indicated
 * by the @authors tag. See the copyright.txt in the distribution for a
 * full listing of individual contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */
package de.ibapl.spsw.tests;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;
import static org.junit.jupiter.api.Assertions.fail;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.text.MessageFormat;
import java.time.Duration;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.AfterTestExecutionCallback;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junit.jupiter.api.extension.ExtensionContext;

import gnu.io.CommPortIdentifier;
import gnu.io.RXTXPort;
import gnu.io.SerialPort;

/**
 * @author Arne Plöse
 */
@ExtendWith(AbstractTwoPortsBaselineRxTxTest.AfterTestExecution.class)
public abstract class AbstractTwoPortsBaselineRxTxTest {

    protected static final int PORT_RECOVERY_TIME_MS = AbstractPortTest.PORT_RECOVERY_TIME_MS;
    protected static final boolean HARDWARE_SUPPORTS_RTS_CTS = false;

    private static final Logger LOG = Logger.getLogger(AbstractTwoPortsBaselineRxTxTest.class.getName());
    private final static int DEFAULT_TEST_BUFFER_SIZE = 128;

    private byte[] dataOut;
    private byte[] dataIn;
    private static String readSerialPortName;
    private static String writeSerialPortName;

    protected RXTXPort readSpc;
    protected RXTXPort writeSpc;

    protected int flowControl = HARDWARE_SUPPORTS_RTS_CTS
            ? (SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT)
            : SerialPort.FLOWCONTROL_NONE;
    protected int parity = SerialPort.PARITY_NONE;
    protected int stopBits = SerialPort.STOPBITS_1;
    protected int dataBits = SerialPort.DATABITS_8;
    protected boolean currentTestFailed;

    public static class AfterTestExecution implements AfterTestExecutionCallback {

        @Override
        public void afterTestExecution(ExtensionContext context) throws Exception {
            ((AbstractTwoPortsBaselineRxTxTest) context.getRequiredTestInstance()).currentTestFailed = context
                    .getExecutionException().isPresent();
        }
    }

    @BeforeAll
    public static void setUpClass() throws Exception {
        try (InputStream is = AbstractTwoPortsBaselineRxTxTest.class.getClassLoader()
                .getResourceAsStream("junit-spsw-config.properties")) {
            if (is == null) {
                readSerialPortName = null;
                writeSerialPortName = null;
            } else {
                Properties p = new Properties();
                p.load(is);
                readSerialPortName = p.getProperty("readPort", null);
                writeSerialPortName = p.getProperty("writePort", null);
            }
        }
    }

    @BeforeEach
    public void setUp() throws Exception {
        if (readSerialPortName != null) {
            CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(readSerialPortName);
            readSpc = (RXTXPort) portIdentifier.open(this.getClass().getName(), 1000);
        }
        if (writeSerialPortName != null) {
            if (writeSerialPortName.equals(readSerialPortName)) {
                writeSpc = readSpc;
            } else {
                CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(writeSerialPortName);
                writeSpc = (RXTXPort) portIdentifier.open(this.getClass().getName(), 1000);
            }
        }
    }

    @AfterEach
    public void tearDown() throws Exception {
        if (writeSpc != null) {
            if (writeSpc != readSpc) {
                writeSpc.close();
            }
            writeSpc = null;
        }
        if (readSpc != null) {
            readSpc.close();
            readSpc = null;
        }

        Runtime.getRuntime().gc();
        Runtime.getRuntime().runFinalization();
        if (currentTestFailed) {
            Thread.sleep(PORT_RECOVERY_TIME_MS);
        }
    }

    private void initBuffers(final int size) {
        dataOut = new byte[size];
        for (int i = 0; i < size; i++) {
            dataOut[i] = (byte) (Math.round(Math.random() * 0xFF) & 0xFF);
        }
        dataIn = new byte[size];
    }

    private void writeBytes() {
        try {
            writeSpc.getOutputStream().write(dataOut);
            LOG.fine("data written");
        } catch (InterruptedIOException iioe) {
            LOG.log(Level.SEVERE, "{0} Bytes send total: {1}",
                    new Object[]{iioe.getMessage(), iioe.bytesTransferred});
            fail(iioe.getMessage() + " Bytes send total: " + iioe.bytesTransferred);
        } catch (IOException e) {
            LOG.severe(e.getMessage());
            fail(e.getMessage());
        }

        int received = 0;
        while (dataIn.length > received) {
            try {
                int length = readSpc.getInputStream().read(dataIn, received, dataIn.length - received);
                if (length >= 0) {
                    received += length;
                } else {
                    throw new RuntimeException();
                }
            } catch (InterruptedIOException iioe) {
                LOG.log(Level.SEVERE, "{0} Bytes received total: {1} Bytes received this: {2}",
                        new Object[]{iioe.getMessage(), received, iioe.bytesTransferred});
                fail(iioe.getMessage() + " Bytes received total: " + received + " Bytes received this: "
                        + iioe.bytesTransferred);
            } catch (IOException e) {
                LOG.severe(e.getMessage());
            }
        }
        assertArrayEquals(dataOut, dataIn);
    }

    private void writeSingle() {
        try {
            for (int i = 0; i < dataOut.length; i++) {
                writeSpc.getOutputStream().write(dataOut[i]);
            }
            LOG.fine("data written");
        } catch (InterruptedIOException iioe) {
            LOG.log(Level.SEVERE, "{0} Bytes send total: {1}",
                    new Object[]{iioe.getMessage(), iioe.bytesTransferred});
            fail(iioe.getMessage() + " Bytes send total: " + iioe.bytesTransferred);
        } catch (IOException e) {
            LOG.severe(e.getMessage());
            fail(e.getMessage());
        }

        for (int i = 0; i < dataIn.length; i++) {
            try {
                int received = readSpc.getInputStream().read();
                if (received >= 0) {
                    dataIn[i] = (byte) received;
                } else {
                    throw new RuntimeException();
                }
            } catch (InterruptedIOException iioe) {
                LOG.log(Level.SEVERE, "{0} Bytes received total: {1} Bytes received this: {2}",
                        new Object[]{iioe.getMessage(), i, iioe.bytesTransferred});
                fail(iioe.getMessage() + " Bytes received total: " + i + " Bytes received this: "
                        + iioe.bytesTransferred);
            } catch (IOException e) {
                LOG.severe(e.getMessage());
            }
        }
        assertArrayEquals(dataOut, dataIn);
    }

    private void runTest(int baudrate, int buffersize) throws Exception {
        assumeTrue(readSpc != null);
        assumeTrue(writeSpc != null);
        LOG.info(MessageFormat.format("do run test @baudrate: {0}, buffer size: {1}", baudrate, buffersize));

        readSpc.setSerialPortParams(baudrate, dataBits, stopBits, parity);
        readSpc.setFlowControlMode(flowControl);
        if (writeSpc != readSpc) {
            writeSpc.setSerialPortParams(baudrate, dataBits, stopBits, parity);
            writeSpc.setFlowControlMode(flowControl);
        }

        printPorts(readSpc, writeSpc);

        int bitsToTransfer = 1 + dataBits;
        switch (stopBits) {
            case SerialPort.STOPBITS_1:
                bitsToTransfer += 1;
                break;
            case SerialPort.STOPBITS_1_5:
            case SerialPort.STOPBITS_2:
                bitsToTransfer += 2;
                break;
            default:
                break;
        }
        bitsToTransfer += (parity == SerialPort.PARITY_NONE ? 0 : 1);

        assertTimeoutPreemptively(Duration.ofMillis(1000 + (dataOut.length * bitsToTransfer * 1000) / baudrate), () -> {
            writeBytes();
        });
        assertTimeoutPreemptively(Duration.ofMillis(1000 + (dataOut.length * bitsToTransfer * 1000) / baudrate), () -> {
            writeSingle();
        });
    }

    private void printPorts(RXTXPort sPort0, RXTXPort sPort1) throws IOException {
        StringBuilder sb = new StringBuilder();
        sb.append(String.format("%n\tName:        %-20s %-20s%n", sPort0.getName(), sPort1.getName()));
        sb.append(String.format("\tBaudrate:    %-20d %-20d%n", sPort0.getBaudRate(), sPort1.getBaudRate()));
        sb.append(String.format("\tStopBits:    %-20d %-20d%n", sPort0.getStopBits(), sPort1.getStopBits()));
        sb.append(String.format("\tParity:      %-20d %-20d%n", sPort0.getParity(), sPort1.getParity()));
        sb.append(
                String.format("\tFlowControl: %-20d %-20d", sPort0.getFlowControlMode(), sPort1.getFlowControlMode()));

        LOG.log(Level.INFO, sb.toString());
    }

    @Test
    public void test_0000300() throws Exception {
        runTest(300, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0000600() throws Exception {
        runTest(600, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0001200() throws Exception {
        runTest(1200, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0002400() throws Exception {
        runTest(2400, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0004800() throws Exception {
        runTest(4800, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0009600() throws Exception {
        runTest(9600, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0019200() throws Exception {
        runTest(19200, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0038400() throws Exception {
        runTest(38400, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0057600() throws Exception {
        runTest(57600, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0115200() throws Exception {
        runTest(115200, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0230400() throws Exception {
        runTest(230400, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0460800() throws Exception {
        runTest(460800, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0500000() throws Exception {
        runTest(500000, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_0576000() throws Exception {
        runTest(576000, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Test
    public void test_1000000() throws Exception {
        runTest(1000000, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Disabled
    @Test
    public void test_1152000() throws Exception {
        runTest(1152000, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Disabled
    @Test
    public void test_2000000() throws Exception {
        runTest(2000000, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Disabled
    @Test
    public void test_3000000() throws Exception {
        runTest(3000000, DEFAULT_TEST_BUFFER_SIZE);
    }

    @Disabled
    @Test
    public void test_4000000() throws Exception {
        runTest(4000000, DEFAULT_TEST_BUFFER_SIZE);
    }

}
