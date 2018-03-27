/*-
 * #%L
 * SPSW Provider
 * %%
 * Copyright (C) 2009 - 2018 Arne Plöse
 * %%
 * SPSW - Drivers for the serial port, https://github.com/aploese/spsw/
 * Copyright (C) 2009-2018, Arne Plöse and individual contributors as indicated
 * by the @authors tag. See the copyright.txt in the distribution for a
 * full listing of individual contributors.
 * 
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as
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
 * #L%
 */
package de.ibapl.spsw.jniprovider;

import java.io.IOException;

/**
 * JNI wrapper around the POSIX termios structure.
 * 
 * Use serial_struct TIOCGSERIAL to get more infos?
 * 
 * @author scream3r
 * @author Arne Plöse
 */
public class GenericTermiosSerialPortSocket extends AbstractSerialPortSocket<GenericTermiosSerialPortSocket> {

	/**
	 * The file descriptor or handle for this Port
	 */
	private volatile int fd = -1;
	/**
	 * The close event file descriptor or handle proper multi threaded closing for
	 * this Port
	 */
	private int closeEventFd = -1;

	private int outByteTime = -1;
	/**
	 * used in native code
	 */
	private int interByteReadTimeout = 100;
	private int pollReadTimeout = -1;
	private int pollWriteTimeout = -1;

	public GenericTermiosSerialPortSocket(String portName) {
		super(portName);
	}

	@Override
	public native void drainOutputBuffer() throws IOException;

	@Override
	public int getInterByteReadTimeout() throws IOException {
		return interByteReadTimeout;
	}

	@Override
	public int getOverallReadTimeout() throws IOException {
		return pollReadTimeout == -1 ? 0 : pollReadTimeout;
	}

	@Override
	public int getOverallWriteTimeout() throws IOException {
		return pollWriteTimeout == -1 ? 0 : pollWriteTimeout;
	}

	public native boolean isDTR() throws IOException;

	public native boolean isRTS() throws IOException;

	@Override
	public native void sendXOFF() throws IOException;

	@Override
	public native void sendXON() throws IOException;

	@Override
	public void setTimeouts(int interByteReadTimeout, int overallReadTimeout, int overallWriteTimeout)
			throws IOException {
		this.interByteReadTimeout = interByteReadTimeout;
		this.pollReadTimeout = overallReadTimeout == 0 ? -1 : overallReadTimeout;
		this.pollWriteTimeout = overallWriteTimeout == 0 ? -1 : overallWriteTimeout;
	}

}
