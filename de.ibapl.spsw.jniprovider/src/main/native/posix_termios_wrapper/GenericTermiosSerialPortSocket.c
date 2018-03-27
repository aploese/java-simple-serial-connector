/*-
 * #%L
 * SPSW Provider
 * %%
 * Copyright (C) 2009 - 2017 Arne Plöse
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

/* jSSC (Java Simple Serial Connector) - serial port communication library.
 * © Alexey Sokolov (scream3r), 2010-2014.
 *
 * This file is part of jSSC.
 *
 * jSSC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jSSC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with jSSC.  If not, see <http://www.gnu.org/licenses/>.
 *
 * If you use jSSC in public project you can inform me about this by e-mail,
 * of course if you want it.
 *
 * e-mail: scream3r.org@gmail.com
 * web-site: http://scream3r.org | http://code.google.com/p/java-simple-serial-connector/
 */
#include <stdlib.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <errno.h>//-D_TS_ERRNO use for Solaris C++ compiler

#ifdef __linux__
#include <linux/serial.h>
#endif
#ifdef __SunOS
#include <sys/filio.h>//Needed for FIONREAD in Solaris
#include <string.h>//Needed for select() function
#endif
#ifdef __APPLE__
#include <serial/ioss.h>//Needed for IOSSIOSPEED in Mac OS X (Non standard speeds)
#endif

#include <poll.h>
//Linux include...
#include <sys/eventfd.h>

#include <jni.h>

#include "de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket.h"
#include "de_ibapl_spsw_jniprovider_AbstractSerialPortSocket.h"

#undef SPSW_SPEED_0_BPS
#define SPSW_SPEED_0_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_0_BPS
#undef SPSW_SPEED_50_BPS
#define SPSW_SPEED_50_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_50_BPS
#undef SPSW_SPEED_75_BPS
#define SPSW_SPEED_75_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_75_BPS
#undef SPSW_SPEED_110_BPS
#define SPSW_SPEED_110_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_110_BPS
#undef SPSW_SPEED_134_BPS
#define SPSW_SPEED_134_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_134_BPS
#undef SPSW_SPEED_150_BPS
#define SPSW_SPEED_150_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_150_BPS
#undef SPSW_SPEED_200_BPS
#define SPSW_SPEED_200_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_200_BPS
#undef SPSW_SPEED_300_BPS
#define SPSW_SPEED_300_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_300_BPS
#undef SPSW_SPEED_600_BPS
#define SPSW_SPEED_600_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_600_BPS
#undef SPSW_SPEED_1200_BPS
#define SPSW_SPEED_1200_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_1200_BPS
#undef SPSW_SPEED_1800_BPS
#define SPSW_SPEED_1800_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_1800_BPS
#undef SPSW_SPEED_2400_BPS
#define SPSW_SPEED_2400_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_2400_BPS
#undef SPSW_SPEED_4800_BPS
#define SPSW_SPEED_4800_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_4800_BPS
#undef SPSW_SPEED_9600_BPS
#define SPSW_SPEED_9600_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_9600_BPS
#undef SPSW_SPEED_19200_BPS
#define SPSW_SPEED_19200_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_19200_BPS
#undef SPSW_SPEED_38400_BPS
#define SPSW_SPEED_38400_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_38400_BPS
#undef SPSW_SPEED_57600_BPS
#define SPSW_SPEED_57600_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_57600_BPS
#undef SPSW_SPEED_115200_BPS
#define SPSW_SPEED_115200_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_115200_BPS
#undef SPSW_SPEED_230400_BPS
#define SPSW_SPEED_230400_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_230400_BPS
#undef SPSW_SPEED_460800_BPS
#define SPSW_SPEED_460800_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_460800_BPS
#undef SPSW_SPEED_500000_BPS
#define SPSW_SPEED_500000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_500000_BPS
#undef SPSW_SPEED_576000_BPS
#define SPSW_SPEED_576000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_576000_BPS
#undef SPSW_SPEED_921600_BPS
#define SPSW_SPEED_921600_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_921600_BPS
#undef SPSW_SPEED_1000000_BPS
#define SPSW_SPEED_1000000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_1000000_BPS
#undef SPSW_SPEED_1152000_BPS
#define SPSW_SPEED_1152000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_1152000_BPS
#undef SPSW_SPEED_1500000_BPS
#define SPSW_SPEED_1500000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_1500000_BPS
#undef SPSW_SPEED_2000000_BPS
#define SPSW_SPEED_2000000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_2000000_BPS
#undef SPSW_SPEED_2500000_BPS
#define SPSW_SPEED_2500000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_2500000_BPS
#undef SPSW_SPEED_3000000_BPS
#define SPSW_SPEED_3000000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_3000000_BPS
#undef SPSW_SPEED_3500000_BPS
#define SPSW_SPEED_3500000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_3500000_BPS
#undef SPSW_SPEED_4000000_BPS
#define SPSW_SPEED_4000000_BPS de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_4000000_BPS
#undef SPSW_SPEED_MASK
#define SPSW_SPEED_MASK de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_SPEED_MASK
#undef SPSW_DATA_BITS_DB5
#define SPSW_DATA_BITS_DB5 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_DATA_BITS_DB5
#undef SPSW_DATA_BITS_DB6
#define SPSW_DATA_BITS_DB6 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_DATA_BITS_DB6
#undef SPSW_DATA_BITS_DB7
#define SPSW_DATA_BITS_DB7 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_DATA_BITS_DB7
#undef SPSW_DATA_BITS_DB8
#define SPSW_DATA_BITS_DB8 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_DATA_BITS_DB8
#undef SPSW_DATA_BITS_MASK
#define SPSW_DATA_BITS_MASK de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_DATA_BITS_MASK
#undef SPSW_FLOW_CONTROL_NONE
#define SPSW_FLOW_CONTROL_NONE de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_FLOW_CONTROL_NONE
#undef SPSW_FLOW_CONTROL_RTS_CTS_IN
#define SPSW_FLOW_CONTROL_RTS_CTS_IN de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_FLOW_CONTROL_RTS_CTS_IN
#undef SPSW_FLOW_CONTROL_RTS_CTS_OUT
#define SPSW_FLOW_CONTROL_RTS_CTS_OUT de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_FLOW_CONTROL_RTS_CTS_OUT
#undef SPSW_FLOW_CONTROL_XON_XOFF_IN
#define SPSW_FLOW_CONTROL_XON_XOFF_IN de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_FLOW_CONTROL_XON_XOFF_IN
#undef SPSW_FLOW_CONTROL_XON_XOFF_OUT
#define SPSW_FLOW_CONTROL_XON_XOFF_OUT de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_FLOW_CONTROL_XON_XOFF_OUT
#undef SPSW_FLOW_CONTROL_MASK
#define SPSW_FLOW_CONTROL_MASK de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_FLOW_CONTROL_MASK
#undef SPSW_STOP_BITS_1
#define SPSW_STOP_BITS_1 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_STOP_BITS_1
#undef SPSW_STOP_BITS_1_5
#define SPSW_STOP_BITS_1_5 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_STOP_BITS_1_5
#undef SPSW_STOP_BITS_2
#define SPSW_STOP_BITS_2 de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_STOP_BITS_2
#undef SPSW_STOP_BITS_MASK
#define SPSW_STOP_BITS_MASK de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_STOP_BITS_MASK
#undef SPSW_PARITY_NONE
#define SPSW_PARITY_NONE de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_PARITY_NONE
#undef SPSW_PARITY_ODD
#define SPSW_PARITY_ODD de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_PARITY_ODD
#undef SPSW_PARITY_EVEN
#define SPSW_PARITY_EVEN de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_PARITY_EVEN
#undef SPSW_PARITY_MARK
#define SPSW_PARITY_MARK de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_PARITY_MARK
#undef SPSW_PARITY_SPACE
#define SPSW_PARITY_SPACE de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_PARITY_SPACE
#undef SPSW_PARITY_MASK
#define SPSW_PARITY_MASK de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_PARITY_MASK
#undef SPSW_NO_PARAMS_TO_SET
#define SPSW_NO_PARAMS_TO_SET de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_NO_PARAMS_TO_SET

#undef INVALID_FD
#define INVALID_FD -1

static jfieldID spsw_portName; /* id for field 'portName'  */
static jfieldID spsw_fd; /* id for field 'fd'  */
static jfieldID spsw_closeEventFd; /* id for field 'closeEventFd'  */
static jfieldID spsw_interByteReadTimeout; // id for field interByteReadTimeout
static jfieldID spsw_pollReadTimeout; // id for field overallReadTimeout
static jfieldID spsw_pollWriteTimeout; // id for field overallWriteTimeout

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	JNIEnv *env;
	if ((*jvm)->GetEnv(jvm, (void **) &env, JNI_VERSION_1_2)) {
		return JNI_ERR;
	}

	//Get field IDs
	jclass genericTermiosSerialPortSocket = (*env)->FindClass(env,
			"de/ibapl/spsw/jniprovider/GenericTermiosSerialPortSocket");
	if (genericTermiosSerialPortSocket == NULL) {
		return JNI_ERR;
	}

	spsw_fd = (*env)->GetFieldID(env, genericTermiosSerialPortSocket, "fd",
			"I");
	if (spsw_fd == NULL) {
		return JNI_ERR;
	}
	spsw_closeEventFd = (*env)->GetFieldID(env, genericTermiosSerialPortSocket,
			"closeEventFd", "I");
	if (spsw_closeEventFd == NULL) {
		return JNI_ERR;
	}
	spsw_interByteReadTimeout = (*env)->GetFieldID(env,
			genericTermiosSerialPortSocket, "interByteReadTimeout", "I");
	if (spsw_interByteReadTimeout == NULL) {
		return JNI_ERR;
	}
	spsw_pollReadTimeout = (*env)->GetFieldID(env,
			genericTermiosSerialPortSocket, "pollReadTimeout", "I");
	if (spsw_pollReadTimeout == NULL) {
		return JNI_ERR;
	}
	spsw_pollWriteTimeout = (*env)->GetFieldID(env,
			genericTermiosSerialPortSocket, "pollWriteTimeout", "I");
	if (spsw_pollWriteTimeout == NULL) {
		return JNI_ERR;
	}
	spsw_portName = (*env)->GetFieldID(env, genericTermiosSerialPortSocket,
			"portName", "Ljava/lang/String;");
	if (spsw_portName == NULL) {
		return JNI_ERR;
	}
	(*env)->DeleteLocalRef(env, genericTermiosSerialPortSocket);

	// mark that the lib was loaded
	jclass serialPortSocketFactoryImpl = (*env)->FindClass(env,
			"de/ibapl/spsw/jniprovider/SerialPortSocketFactoryImpl");
	if (serialPortSocketFactoryImpl == NULL) {
		return JNI_ERR;
	}
	jfieldID spsw_libLoaded = (*env)->GetStaticFieldID(env,
			serialPortSocketFactoryImpl, "libLoaded", "Z");
	if (spsw_libLoaded == NULL) {
		return JNI_ERR;
	}

	(*env)->SetStaticBooleanField(env, serialPortSocketFactoryImpl,
			spsw_libLoaded, JNI_TRUE);

	(*env)->DeleteLocalRef(env, serialPortSocketFactoryImpl);

	return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnLoad(JavaVM *jvm, void *reserved) {
	JNIEnv *env;
	spsw_fd = 0;
	spsw_closeEventFd = 0;
	spsw_interByteReadTimeout = 0;
	spsw_pollReadTimeout = 0;
	spsw_pollWriteTimeout = 0;
	spsw_portName = 0;

	if ((*jvm)->GetEnv(jvm, (void **) &env, JNI_VERSION_1_2)) {
		return;
	}

	// mark that the lib was unloaded
	jclass serialPortSocketFactoryImpl = (*env)->FindClass(env, "de/ibapl/spsw/jniprovider/SerialPortSocketFactoryImpl");
	if (serialPortSocketFactoryImpl == NULL) {
		return;
	}
	jfieldID spsw_libLoaded = (*env)->GetStaticFieldID(env, serialPortSocketFactoryImpl, "libLoaded", "Z");
	if (spsw_libLoaded == NULL) {
		return;
	}
	(*env)->SetStaticBooleanField(env, serialPortSocketFactoryImpl, spsw_libLoaded, JNI_FALSE);
	(*env)->DeleteLocalRef(env, serialPortSocketFactoryImpl);

}

// Helper method

static void throw_IOException_NativeError(JNIEnv *env, const char *msg) {
	char buf[2048];
	snprintf(buf, 2048, "%s: Unknown port error %d: (%s)", msg, errno,
			strerror(errno));
	const jclass ioeClass = (*env)->FindClass(env,
			"java/io/IOException");
	if (ioeClass != NULL) {
		(*env)->ThrowNew(env, ioeClass, buf);
		(*env)->DeleteLocalRef(env, ioeClass);
	}
}

static void throw_Illegal_Argument_Exception(JNIEnv *env, const char *msg) {
	const jclass iaeClass = (*env)->FindClass(env,
			"java/lang/IllegalArgumentException");
	if (iaeClass != NULL) {
		(*env)->ThrowNew(env, iaeClass, msg);
		(*env)->DeleteLocalRef(env, iaeClass);
	}
}

static void throw_IOException(JNIEnv *env, const char* msg, jstring portName) {
	const char* port = (*env)->GetStringUTFChars(env, portName, JNI_FALSE);
	char buf[2048];
	snprintf(buf, sizeof(buf), msg, port);
	(*env)->ReleaseStringUTFChars(env, portName, port);

	const jclass ioeClass = (*env)->FindClass(env,
			"java/io/IOException");
	if (ioeClass != NULL) {
		(*env)->ThrowNew(env, ioeClass, buf);
		(*env)->DeleteLocalRef(env, ioeClass);
	}
}

static void throw_PortBusyException(JNIEnv *env, jstring portName) {
	throw_IOException(env, "Port is busy: (%s)", portName);
}

static void throw_PortNotFoundException(JNIEnv *env, jstring portName) {
	throw_IOException(env, "Port not found: (%s)", portName);
}

static void throw_PermissionDeniedException(JNIEnv *env, jstring portName) {
	throw_IOException(env, "Permission denied: (%s)", portName);
}

static void throw_NotASerialPortException(JNIEnv *env, jstring portName) {
	throw_IOException(env, "Not a serial port: (%s)", portName);
}

static void throw_IOException_Opend(JNIEnv *env) {
	const jclass spsClass = (*env)->FindClass(env,
			"de/ibapl/spsw/api/SerialPortSocket");
	const jclass ioeClass = (*env)->FindClass(env,
			"java/io/IOException");

	if ((ioeClass != NULL) || (spsClass != NULL)) {

		const jfieldID PORT_IS_OPEN = (*env)->GetStaticFieldID(env, spsClass,
				"PORT_IS_OPEN", "Ljava/lang/String;");
		const jmethodID ioeConstructor = (*env)->GetMethodID(env, ioeClass,
				"<init>", "(Ljava/lang/String;)V");
		const jobject ioeEx = (*env)->NewObject(env, ioeClass, ioeConstructor,
				(*env)->GetStaticObjectField(env, spsClass, PORT_IS_OPEN));

		(*env)->Throw(env, ioeEx);

		(*env)->DeleteLocalRef(env, ioeClass);
		(*env)->DeleteLocalRef(env, spsClass);
	}
}

static void throw_IOException_Closed(JNIEnv *env) {
	const jclass spsClass = (*env)->FindClass(env,
			"de/ibapl/spsw/api/SerialPortSocket");
	const jclass ioeClass = (*env)->FindClass(env,
			"java/io/IOException");

	if ((ioeClass != NULL) || (spsClass != NULL)) {

		const jfieldID PORT_IS_CLOSED = (*env)->GetStaticFieldID(env, spsClass,
				"PORT_IS_CLOSED", "Ljava/lang/String;");
		const jmethodID ioeConstructor = (*env)->GetMethodID(env, ioeClass,
				"<init>", "(Ljava/lang/String;)V");
		const jobject ioeEx = (*env)->NewObject(env, ioeClass, ioeConstructor,
				(*env)->GetStaticObjectField(env, spsClass, PORT_IS_CLOSED));

		(*env)->Throw(env, ioeEx);

		(*env)->DeleteLocalRef(env, ioeClass);
		(*env)->DeleteLocalRef(env, spsClass);
	}
}

static void throw_IO_ClosedException(JNIEnv *env, int bytesTransferred) {
	const jclass spsClass = (*env)->FindClass(env,
			"de/ibapl/spsw/api/SerialPortSocket");
	const jclass iioeClass = (*env)->FindClass(env,
			"java/io/InterruptedIOException");

	if ((iioeClass != NULL) && (spsClass != NULL)) {

		const jfieldID PORT_IS_CLOSED = (*env)->GetStaticFieldID(env, spsClass,
				"PORT_IS_CLOSED", "Ljava/lang/String;");
		const jmethodID iioeConstructor = (*env)->GetMethodID(env, iioeClass,
				"<init>", "(Ljava/lang/String;)V");
		const jobject iioeEx = (*env)->NewObject(env, iioeClass,
				iioeConstructor,
				(*env)->GetStaticObjectField(env, spsClass, PORT_IS_CLOSED));
		const jfieldID bytesTransferredId = (*env)->GetFieldID(env, iioeClass,
				"bytesTransferred", "I");
		(*env)->SetIntField(env, iioeEx, bytesTransferredId, bytesTransferred);
		(*env)->Throw(env, iioeEx);
		(*env)->DeleteLocalRef(env, iioeClass);
	}
}

static void throw_InterruptedIOExceptionWithError(JNIEnv *env,
		int bytesTransferred, const char *msg) {
	char buf[2048];
	snprintf(buf, 2048, "%s: Unknown port error %d: (%s)", msg, errno,
			strerror(errno));
	const jclass iioeClass = (*env)->FindClass(env,
			"java/io/InterruptedIOException");
	if (iioeClass != NULL) {
		const jmethodID iioeConstructor = (*env)->GetMethodID(env, iioeClass,
				"<init>", "(Ljava/lang/String;)V");
		const jobject iioeEx = (*env)->NewObject(env, iioeClass,
				iioeConstructor, (*env)->NewStringUTF(env, buf));
		const jfieldID bytesTransferredId = (*env)->GetFieldID(env, iioeClass,
				"bytesTransferred", "I");
		(*env)->SetIntField(env, iioeEx, bytesTransferredId, bytesTransferred);
		(*env)->Throw(env, iioeEx);
		(*env)->DeleteLocalRef(env, iioeClass);
	}
}

static void throw_TimeoutIOException(JNIEnv *env, int bytesTransferred) {
	const jclass tioeClass = (*env)->FindClass(env,
			"de/ibapl/spsw/api/TimeoutIOException");
	if (tioeClass != NULL) {
		const jmethodID tioeConstructor = (*env)->GetMethodID(env, tioeClass,
				"<init>", "(Ljava/lang/String;)V");
		const jobject tioeEx = (*env)->NewObject(env, tioeClass,
				tioeConstructor, (*env)->NewStringUTF(env, "Timeout"));
		const jfieldID bytesTransferredId = (*env)->GetFieldID(env, tioeClass,
				"bytesTransferred", "I");
		(*env)->SetIntField(env, tioeEx, bytesTransferredId, bytesTransferred);
		(*env)->Throw(env, tioeEx);
		(*env)->DeleteLocalRef(env, tioeClass);
	}
}

static void throw_IO_ClosedOrInterruptedException(JNIEnv *env, jobject sps,
		int bytesTransferred, const char *message) {
	if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
		throw_IO_ClosedException(env, bytesTransferred);
	} else {
		throw_InterruptedIOExceptionWithError(env, bytesTransferred, message);
	}
}

static void throw_ClosedOrNativeException(JNIEnv *env, jobject sps,
		const char *message) {
	if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
		throw_IOException_Closed(env);
	} else {
		throw_IOException_NativeError(env, message);
	}
}

static jint speed_t2Speed(JNIEnv *env, speed_t speed) {
	switch (speed) {
	case B0:
		return SPSW_SPEED_0_BPS;
	case B50:
		return SPSW_SPEED_50_BPS;
	case B75:
		return SPSW_SPEED_75_BPS;
	case B110:
		return SPSW_SPEED_110_BPS;
	case B134:
		return SPSW_SPEED_134_BPS;
	case B150:
		return SPSW_SPEED_150_BPS;
	case B200:
		return SPSW_SPEED_200_BPS;
	case B300:
		return SPSW_SPEED_300_BPS;
	case B600:
		return SPSW_SPEED_600_BPS;
	case B1200:
		return SPSW_SPEED_1200_BPS;
	case B1800:
		return SPSW_SPEED_1800_BPS;
	case B2400:
		return SPSW_SPEED_2400_BPS;
	case B4800:
		return SPSW_SPEED_4800_BPS;
	case B9600:
		return SPSW_SPEED_9600_BPS;
	case B19200:
		return SPSW_SPEED_19200_BPS;
	case B38400:
		return SPSW_SPEED_38400_BPS;
#ifdef B57600
		case B57600:
		return SPSW_SPEED_57600_BPS;
#endif
#ifdef B115200
		case B115200:
		return SPSW_SPEED_115200_BPS;
#endif
#ifdef B230400
		case B230400:
		return SPSW_SPEED_230400_BPS;
#endif
#ifdef B460800
		case B460800:
		return SPSW_SPEED_460800_BPS;
#endif
#ifdef B500000
		case B500000:
		return SPSW_SPEED_500000_BPS;
#endif
#ifdef B576000
		case B576000:
		return SPSW_SPEED_576000_BPS;
#endif
#ifdef B921600
		case B921600:
		return SPSW_SPEED_921600_BPS;
#endif
#ifdef B1000000
		case B1000000:
		return SPSW_SPEED_1000000_BPS;
#endif
#ifdef B1152000
		case B1152000:
		return SPSW_SPEED_1152000_BPS;
#endif
#ifdef B1500000
		case B1500000:
		return SPSW_SPEED_1500000_BPS;
#endif
#ifdef B2000000
		case B2000000:
		return SPSW_SPEED_2000000_BPS;
#endif
#ifdef B2500000
		case B2500000:
		return SPSW_SPEED_2500000_BPS;
#endif
#ifdef B3000000
		case B3000000:
		return SPSW_SPEED_3000000_BPS;
#endif
#ifdef B3500000
		case B3500000:
		return SPSW_SPEED_3500000_BPS;
#endif
#ifdef B4000000
		case B4000000:
		return SPSW_SPEED_4000000_BPS;
#endif
	default:
		throw_Illegal_Argument_Exception(env, "speed not supported");
		return -1;
	}
}

static jint speed2speed_t(JNIEnv *env, jint speed, speed_t *speedValue) {
	switch (speed) {
	case SPSW_SPEED_0_BPS:
		*speedValue = B0;
		break;
	case SPSW_SPEED_50_BPS:
		*speedValue = B50;
		break;
	case SPSW_SPEED_75_BPS:
		*speedValue = B75;
		break;
	case SPSW_SPEED_110_BPS:
		*speedValue = B110;
		break;
	case SPSW_SPEED_134_BPS:
		*speedValue = B134;
		break;
	case SPSW_SPEED_150_BPS:
		*speedValue = B150;
		break;
	case SPSW_SPEED_200_BPS:
		*speedValue = B200;
		break;
	case SPSW_SPEED_300_BPS:
		*speedValue = B300;
		break;
	case SPSW_SPEED_600_BPS:
		*speedValue = B600;
		break;
	case SPSW_SPEED_1200_BPS:
		*speedValue = B1200;
		break;
	case SPSW_SPEED_1800_BPS:
		*speedValue = B1800;
		break;
	case SPSW_SPEED_2400_BPS:
		*speedValue = B2400;
		break;
	case SPSW_SPEED_4800_BPS:
		*speedValue = B4800;
		break;
	case SPSW_SPEED_9600_BPS:
		*speedValue = B9600;
		break;
	case SPSW_SPEED_19200_BPS:
		*speedValue = B19200;
		break;
	case SPSW_SPEED_38400_BPS:
		*speedValue = B38400;
		break;
#ifdef B57600
		case SPSW_SPEED_57600_BPS:
			*speedValue = B57600;
		break;
#endif
#ifdef B115200
		case SPSW_SPEED_115200_BPS:
			*speedValue = B115200;
		break;
#endif
#ifdef B230400
		case SPSW_SPEED_230400_BPS:
			*speedValue = B230400;
		break;
#endif
#ifdef B460800
		case SPSW_SPEED_460800_BPS:
			*speedValue = B460800;
		break;
#endif
#ifdef B500000
		case SPSW_SPEED_500000_BPS:
			*speedValue = B500000;
		break;
#endif
#ifdef B576000
		case SPSW_SPEED_576000_BPS:
			*speedValue = B576000;
		break;
#endif
#ifdef B921600
		case SPSW_SPEED_921600_BPS:
			*speedValue = B921600;
		break;
#endif
#ifdef B1000000
		case SPSW_SPEED_1000000_BPS:
			*speedValue = B1000000;
		break;
#endif
#ifdef B1152000
		case SPSW_SPEED_1152000_BPS:
			*speedValue = B1152000;
		break;
#endif
#ifdef B1500000
		case SPSW_SPEED_1500000_BPS:
			*speedValue = B1500000;
		break;
#endif
#ifdef B2000000
		case SPSW_SPEED_2000000_BPS:
			*speedValue = B2000000;
		break;
#endif
#ifdef B2500000
		case SPSW_SPEED_2500000_BPS:
			*speedValue = B2500000;
		break;
#endif
#ifdef B3000000
		case SPSW_SPEED_3000000_BPS:
			*speedValue = B3000000;
		break;
#endif
#ifdef B3500000
		case SPSW_SPEED_3500000_BPS:
			*speedValue = B3500000;
		break;
#endif
#ifdef B4000000
		case SPSW_SPEED_4000000_BPS:
			*speedValue = B4000000;
		break;
#endif
	default:
		throw_Illegal_Argument_Exception(env, "Speed not supported");
		return -1;
	}
	return 0;
}

static int getParams(JNIEnv *env, jobject sps, jint* paramBitSet) {
	int fd = (*env)->GetIntField(env, sps, spsw_fd);
	struct termios settings;
	if (tcgetattr(fd, &settings)) {
		throw_ClosedOrNativeException(env, sps, "setParams tcsetattr");
		return -1;
	}

	jint result = 0;
	//Speed
	if (*paramBitSet & SPSW_SPEED_MASK) {
		//Get standard speed from "termios.h"
		speed_t inSpeed = cfgetispeed(&settings);
		speed_t outSpeed = cfgetospeed(&settings);
		if (inSpeed != outSpeed) {
			throw_IOException_NativeError(env,
					"In and out speed mismatch");
			return -1;
		}
		jint speed = speed_t2Speed(env, inSpeed);
		if (speed == -1) {
			//IAE is already thrown...
			return -1;
		} else {
			result |= speed;
		}

	}

	//DataBits
	if (*paramBitSet & SPSW_DATA_BITS_MASK) {
		switch (settings.c_cflag & CSIZE) {
		case CS5:
			result |= SPSW_DATA_BITS_DB5;
			break;
		case CS6:
			result |= SPSW_DATA_BITS_DB6;
			break;
		case CS7:
			result |= SPSW_DATA_BITS_DB7;
			break;
		case CS8:
			result |= SPSW_DATA_BITS_DB8;
			break;
		default:
			throw_Illegal_Argument_Exception(env, "Unknown databits");
			return -1;
		}
	}

	//StopBits
	if (*paramBitSet & SPSW_STOP_BITS_MASK) {
		if ((settings.c_cflag & CSTOPB) == 0) {
			result |= SPSW_STOP_BITS_1;
		} else if ((settings.c_cflag & CSTOPB) == CSTOPB) {
			if ((settings.c_cflag & CSIZE) == CS5) {
				result |= SPSW_STOP_BITS_1_5;
			} else {
				result |= SPSW_STOP_BITS_2;
			}
		}
	}

	//Parity
	if (*paramBitSet & SPSW_PARITY_MASK) {
		if ((settings.c_cflag & PARENB) == 0) {
			result |= SPSW_PARITY_NONE;

		} else if ((settings.c_cflag & PARODD) == 0) {
			// EVEN or SPACE
#ifdef PAREXT
			if ((settings.c_cflag & PAREXT) == 0) {
				result |= SPSW_PARITY_EVEN;
			} else {
				result |= SPSW_PARITY_SPACE;
			}
#elif defined CMSPAR
			if ((settings.c_cflag & CMSPAR) == 0) {
				result |= SPSW_PARITY_EVEN;
			} else {
				result |= SPSW_PARITY_SPACE;
			}
#endif
		} else {
			// ODD or MARK
#ifdef PAREXT
			if ((settings.c_cflag & PAREXT) == 0) {
				result |= SPSW_PARITY_ODD;
			} else {
				result |= SPSW_PARITY_MARK;
			}
#elif defined CMSPAR
			if ((settings.c_cflag & CMSPAR) == 0) {
				result |= SPSW_PARITY_ODD;
			} else {
				result |= SPSW_PARITY_MARK;
			}
#endif
		}
		if (!(result & SPSW_PARITY_MASK)) {
			//Parity not found
			throw_IOException_NativeError(env,
					"getParam could not figure out Parity");
			return -1;
		}
	}

	//FlowControl
	if (*paramBitSet & SPSW_FLOW_CONTROL_MASK) {
		result |= SPSW_FLOW_CONTROL_NONE;
		if (settings.c_cflag & CRTSCTS) {
			result |= (SPSW_FLOW_CONTROL_RTS_CTS_IN
					| SPSW_FLOW_CONTROL_RTS_CTS_OUT);
			result &= ~SPSW_FLOW_CONTROL_NONE; //clean NONE
		}
		if (settings.c_iflag & IXOFF) {
			result |= SPSW_FLOW_CONTROL_XON_XOFF_IN;
			result &= ~SPSW_FLOW_CONTROL_NONE; //clean NONE
		}
		if (settings.c_iflag & IXON) {
			result |= SPSW_FLOW_CONTROL_XON_XOFF_OUT;
			result &= ~SPSW_FLOW_CONTROL_NONE; //clean NONE
		}
	}
	*paramBitSet = result;
	return 0;
}

static int setParams(JNIEnv *env, jobject sps, struct termios *settings,
		jint paramBitSet) {

	//Speed
	if (paramBitSet & SPSW_SPEED_MASK) {
		speed_t speedValue;
		if (speed2speed_t(env, paramBitSet & SPSW_SPEED_MASK, &speedValue) == -1) {
			//IAE is already thrown...
			return -1;
		}

		//Set standard speed from "termios.h"
		if (cfsetspeed(settings, speedValue) < 0) {
			throw_ClosedOrNativeException(env, sps,
					"Can't set Speed cfsetspeed(settings, speedValue)");
			return -1;
		}

	}

	//DataBits
	if (paramBitSet & SPSW_DATA_BITS_MASK) {
		settings->c_cflag &= ~CSIZE;
		switch (paramBitSet & SPSW_DATA_BITS_MASK) {
		case SPSW_DATA_BITS_DB5:
			settings->c_cflag |= CS5;
			break;
		case SPSW_DATA_BITS_DB6:
			settings->c_cflag |= CS6;
			break;
		case SPSW_DATA_BITS_DB7:
			settings->c_cflag |= CS7;
			break;
		case SPSW_DATA_BITS_DB8:
			settings->c_cflag |= CS8;
			break;
		default:
			throw_Illegal_Argument_Exception(env, "Wrong databits");
			return -1;
		}
	}

	//StopBits
	if (paramBitSet & SPSW_STOP_BITS_MASK) {
		switch (paramBitSet & SPSW_STOP_BITS_MASK) {
		case SPSW_STOP_BITS_1:
			//1 stop bit (for info see ->> MSDN)
			settings->c_cflag &= ~CSTOPB;
			break;
		case SPSW_STOP_BITS_1_5:
			if ((settings->c_cflag & CSIZE) == CS5) {
				settings->c_cflag |= CSTOPB;
			} else {
				throw_Illegal_Argument_Exception(env,
						"setStopBits 1.5 stop bits are only valid for 5 DataBits");
				return -1;
			}
			break;
		case SPSW_STOP_BITS_2:
			if ((settings->c_cflag & CSIZE) == CS5) {
				throw_Illegal_Argument_Exception(env,
						"setStopBits 2 stop bits are only valid for 6,7,8 DataBits");
				return -1;
			} else {
				settings->c_cflag |= CSTOPB;
			}
			break;
		default:
			throw_IOException_NativeError(env, "Unknown stopbits");
			return -1;
		}
	}

	//Parity
	if (paramBitSet & SPSW_PARITY_MASK) {
#ifdef PAREXT
		settings->c_cflag &= ~(PARENB | PARODD | PAREXT); //Clear parity settings
#elif defined CMSPAR
		settings->c_cflag &= ~(PARENB | PARODD | CMSPAR); //Clear parity settings
#else
		settings->c_cflag &= ~(PARENB | PARODD); //Clear parity settings
#endif
		switch (paramBitSet & SPSW_PARITY_MASK) {
		case SPSW_PARITY_NONE:
			//Parity NONE
			settings->c_iflag &= ~INPCK; // switch parity input checking off
			break;
		case SPSW_PARITY_ODD:
			//Parity ODD
			settings->c_cflag |= (PARENB | PARODD);
			settings->c_iflag |= INPCK; // switch parity input checking On
			break;
		case SPSW_PARITY_EVEN:
			//Parity EVEN
			settings->c_cflag |= PARENB;
			settings->c_iflag |= INPCK;
			break;
		case SPSW_PARITY_MARK:
			//Parity MARK
#ifdef PAREXT
			settings->c_cflag |= (PARENB | PARODD | PAREXT);
			settings->c_iflag |= INPCK;
#elif defined CMSPAR
			settings->c_cflag |= (PARENB | PARODD | CMSPAR);
			settings->c_iflag |= INPCK;
#endif
			break;
		case SPSW_PARITY_SPACE:
			//Parity SPACE
#ifdef PAREXT
			settings->c_cflag |= (PARENB | PAREXT);
			settings->c_iflag |= INPCK;
#elif defined CMSPAR
			settings->c_cflag |= (PARENB | CMSPAR);
			settings->c_iflag |= INPCK;
#endif
			break;
		default:
			throw_Illegal_Argument_Exception(env, "Wrong parity");
			return -1;
		}
	}

	//FlowControl
	if (paramBitSet & SPSW_FLOW_CONTROL_MASK) {
		jint mask = paramBitSet & SPSW_FLOW_CONTROL_MASK;
		settings->c_cflag &= ~CRTSCTS;
		settings->c_iflag &= ~(IXON | IXOFF);
		if (mask != SPSW_FLOW_CONTROL_NONE) {
			if (((mask & SPSW_FLOW_CONTROL_RTS_CTS_IN)
					== SPSW_FLOW_CONTROL_RTS_CTS_IN)
					|| ((mask & SPSW_FLOW_CONTROL_RTS_CTS_OUT)
							== SPSW_FLOW_CONTROL_RTS_CTS_OUT)) {
				if (((mask & SPSW_FLOW_CONTROL_RTS_CTS_IN)
					== SPSW_FLOW_CONTROL_RTS_CTS_IN)
					&& ((mask & SPSW_FLOW_CONTROL_RTS_CTS_OUT)
							== SPSW_FLOW_CONTROL_RTS_CTS_OUT)) {
					settings->c_cflag |= CRTSCTS;
				} else {
					throw_Illegal_Argument_Exception(env, "Can only set RTS/CTS for both in and out");
					return -1;
				}
			}
			if ((mask & SPSW_FLOW_CONTROL_XON_XOFF_IN)
					== SPSW_FLOW_CONTROL_XON_XOFF_IN) {
				settings->c_iflag |= IXOFF;
			}
			if ((mask & SPSW_FLOW_CONTROL_XON_XOFF_OUT)
					== SPSW_FLOW_CONTROL_XON_XOFF_OUT) {
				settings->c_iflag |= IXON;
			}
		}
	}

	int fd = (*env)->GetIntField(env, sps, spsw_fd);
	//Write the changes...
	if (tcsetattr(fd, TCSANOW, settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "setParams tcsetattr");
		return -1;
	}

	jint paramsRead =
	SPSW_SPEED_MASK & paramBitSet ? SPSW_SPEED_MASK : 0;
	paramsRead |= SPSW_DATA_BITS_MASK & paramBitSet ? SPSW_DATA_BITS_MASK : 0;
	paramsRead |= SPSW_STOP_BITS_MASK & paramBitSet ? SPSW_STOP_BITS_MASK : 0;
	paramsRead |= SPSW_PARITY_MASK & paramBitSet ? SPSW_PARITY_MASK : 0;
	paramsRead |=
	SPSW_FLOW_CONTROL_MASK & paramBitSet ? SPSW_FLOW_CONTROL_MASK : 0;

	if (getParams(env, sps, &paramsRead)) {
		return -1;
	}

	if (paramsRead != paramBitSet) {
		char buf[512];
		if ((paramsRead & SPSW_SPEED_MASK)
				!= (paramBitSet & SPSW_SPEED_MASK)) {
			snprintf(buf, sizeof(buf),
					"Could not set Speed! NATIVE: paramsRead(0x%08lx) != paramBitSet(0x%08lx)",
					paramsRead, paramBitSet);
		} else if ((paramsRead & SPSW_DATA_BITS_MASK)
				!= (paramBitSet & SPSW_DATA_BITS_MASK)) {
			snprintf(buf, sizeof(buf),
					"Could not set DataBits! NATIVE: paramsRead(0x%08lx) != paramBitSet(0x%08lx)",
					paramsRead, paramBitSet);
		} else if ((paramsRead & SPSW_STOP_BITS_MASK)
				!= (paramBitSet & SPSW_STOP_BITS_MASK)) {
			snprintf(buf, sizeof(buf),
					"Could not set StopBits! NATIVE: paramsRead(0x%08lx) != paramBitSet(0x%08lx)",
					paramsRead, paramBitSet);
		} else if ((paramsRead & SPSW_PARITY_MASK)
				!= (paramBitSet & SPSW_PARITY_MASK)) {
			snprintf(buf, sizeof(buf),
					"Could not set Parity! NATIVE: paramsRead(0x%08lx) != paramBitSet(0x%08lx)",
					paramsRead, paramBitSet);
		} else if ((paramsRead & SPSW_FLOW_CONTROL_MASK)
				!= (paramBitSet & SPSW_FLOW_CONTROL_MASK)) {
			snprintf(buf, sizeof(buf),
					"Could not set FlowControl! NATIVE: paramsRead(0x%08lx) != paramBitSet(0x%08lx)",
					paramsRead, paramBitSet);
		} else {
			snprintf(buf, sizeof(buf),
					"Could not set unknown parameter! NATIVE: paramsRead(0x%08lx) != paramBitSet(0x%08lx)",
					paramsRead, paramBitSet);

		}
		throw_Illegal_Argument_Exception(env, buf);
		return -1;
	}
	return 0;
}

static jboolean getLineStatus(JNIEnv *env, jobject sps, int bitMask) {
	int fd = (*env)->GetIntField(env, sps, spsw_fd);
	int lineStatus;
	if (ioctl(fd, TIOCMGET, &lineStatus) != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't get line status");
		return JNI_FALSE;
	}
	if ((lineStatus & bitMask) == bitMask) {
		return JNI_TRUE;
	} else {
		return JNI_FALSE;
	}
}

static void setLineStatus(JNIEnv *env, jobject sps, jboolean enabled,
		int bitMask) {
	int fd = (*env)->GetIntField(env, sps, spsw_fd);
	int lineStatus;
	if (ioctl(fd, TIOCMGET, &lineStatus) != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't get line status");
		return;
	}
	if (enabled == JNI_TRUE) {
		lineStatus |= bitMask;
	} else {
		lineStatus &= ~bitMask;
	}
	if (ioctl(fd, TIOCMSET, &lineStatus) != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't set line status");
	}
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    close0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_close0(
		JNIEnv *env, jobject sps) {
//Mark port as closed...
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);
	(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);

//unlock read/write polls that wait
	const int close_event_fd = (*env)->GetIntField(env, sps, spsw_closeEventFd);
	jbyte evt_buff[8];
	evt_buff[5] = 1;
	evt_buff[6] = 1;
	evt_buff[7] = 1;
	write(close_event_fd, &evt_buff, 8);

	usleep(1000); //1ms
//cleanup
	(*env)->SetIntField(env, sps, spsw_closeEventFd, INVALID_FD);

	if (tcflush(fd, TCIOFLUSH) != 0) {
		//        perror("NATIVE Error Close - tcflush");
	}

	close(close_event_fd);
	if (close(fd) != 0) {
		throw_IOException_NativeError(env, "close0 closing port");
	}
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    getInBufferBytesCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getInBufferBytesCount(
		JNIEnv *env, jobject sps) {
	int fd = (*env)->GetIntField(env, sps, spsw_fd);
	jint returnValue = -1;
	int result = ioctl(fd, FIONREAD, &returnValue);
	if (result != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't read in buffer size");
	}
	return returnValue;
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    getOutBufferBytesCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getOutBufferBytesCount(
		JNIEnv *env, jobject sps) {
	int fd = (*env)->GetIntField(env, sps, spsw_fd);
	jint returnValue = -1;
	int result = ioctl(fd, TIOCOUTQ, &returnValue);
	if (result != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't read out buffer size");
	}
	return returnValue;
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    getParameters
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getParameters(
		JNIEnv *env, jobject sps, jint parameterBitSetMask) {
	if (getParams(env, sps, &parameterBitSetMask)) {
		return 0;
	}
	return parameterBitSetMask;
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    getXOFFChar
 * Signature: ()C
 */
JNIEXPORT jchar JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getXOFFChar(
		JNIEnv *env, jobject sps) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);

	struct termios settings;
	if (tcgetattr(fd, &settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "getXOFFChar tcgetattr");
		return 0;
	}
	return settings.c_cc[VSTOP];

}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    getXONChar
 * Signature: ()C
 */
JNIEXPORT jchar JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getXONChar(
		JNIEnv *env, jobject sps) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);

	struct termios settings;
	if (tcgetattr(fd, &settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "getXONChar tcgetattr");
		return 0;
	}
	return settings.c_cc[VSTART];

}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    isCTS
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isCTS(
		JNIEnv *env, jobject sps) {
	return getLineStatus(env, sps, TIOCM_CTS);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    isDSR
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isDSR(
		JNIEnv *env, jobject sps) {
	return getLineStatus(env, sps, TIOCM_DSR);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    isDCD
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isDCD(
		JNIEnv *env, jobject sps) {
	return getLineStatus(env, sps, TIOCM_CAR);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    isRI
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isRI(
		JNIEnv *env, jobject sps) {
	return getLineStatus(env, sps, TIOCM_RNG);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    open
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_open
(JNIEnv *env, jobject sps, jstring portName, jint paramBitSet) {

//Do not try to reopen port and therefore failing and overriding the file descriptor
	if ((*env)->GetIntField(env, sps, spsw_fd) != INVALID_FD) {
		throw_IOException_Opend(env);
		return;
	}

	const char* port = (*env)->GetStringUTFChars(env, portName, JNI_FALSE);
	int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);

	(*env)->ReleaseStringUTFChars(env, portName, port);

	if (fd == INVALID_FD) {
		(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);
		switch (errno) {
			case EBUSY:
			throw_PortBusyException(env, portName);
			break;
			case ENOENT:
			throw_PortNotFoundException(env, portName);
			break;
			case EACCES:
			throw_PermissionDeniedException(env, portName);
			break;
			case EIO:
			throw_NotASerialPortException(env, portName);
			break;
			default:
			throw_IOException_NativeError(env, "open");
		}
		return;
	}

	// The port is open, but maybe not configured ... setParam and getParam needs this to be set for their field access
	(*env)->SetIntField(env, sps, spsw_fd, fd);

//check termios structure for separating real serial devices from others
	struct termios settings;
	if (tcgetattr(fd, &settings)) {
		close(fd); //since 2.7.0
		(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);
		switch (errno) {
			case ENOTTY:
			throw_NotASerialPortException(env, portName);
			break;
			default:
			throw_IOException_NativeError(env, "open tcgetattr");
		}
		return;
	}

// Yes we use this port exclusively
	if (ioctl(fd, TIOCEXCL) != 0) {
		close(fd);
		(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);
		throw_IOException_NativeError(env, "Can't set exclusive access");
		return;
	}

//set basic settings
	settings.c_cflag |= (CREAD | CLOCAL);
	settings.c_lflag = 0;
	/* Raw input*/
	settings.c_iflag = 0;
	/* Raw output */
	settings.c_oflag = 0;
	settings.c_cc[VMIN] = 0; // If there is not anything just pass
	settings.c_cc[VTIME] = 0;// No timeout

	if (paramBitSet != SPSW_NO_PARAMS_TO_SET) {
		//set Speed etc.
		if (setParams(env, sps, &settings, paramBitSet)) {
			close(fd);
			(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);
			return;
		}
	} else {

		if (tcsetattr(fd, TCSANOW, &settings) != 0) {
			close(fd);
			(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);
			throw_IOException_NativeError(env, "Can't call tcsetattr TCSANOW");
			return;
		}
	}

// flush the device
	if (tcflush(fd, TCIOFLUSH) != 0) {
		close(fd);
		(*env)->SetIntField(env, sps, spsw_fd, INVALID_FD);
		throw_IOException_NativeError(env, "Can't flush device");
		return;
	}

//on linux to avoid read/close problem maybe this helps?

	int close_event_fd = eventfd(0, EFD_NONBLOCK);//counter is zero so nothing to read is available
	if (close_event_fd == INVALID_FD) {
		close(fd);
		throw_IOException_NativeError(env, "Can't create close_event_fd");
		return;
	}

	(*env)->SetIntField(env, sps, spsw_closeEventFd, close_event_fd);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    readBytes
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_readBytes(
		JNIEnv *env, jobject sps, jbyteArray bytes, jint off, jint len) {

	jbyte *lpBuffer = (jbyte*) malloc(len);
	struct pollfd fds[2];
	fds[0].fd = (*env)->GetIntField(env, sps, spsw_fd);
	fds[0].events = POLLIN;
	fds[1].fd = (*env)->GetIntField(env, sps, spsw_closeEventFd);
	fds[1].events = POLLIN;

	int nread = read(fds[0].fd, lpBuffer, len);
	if (nread < 0) {
		if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
			//Filehandle not valid -> closed.
		} else {
			throw_InterruptedIOExceptionWithError(env, 0,
					"readBytes: read error during first invocation of read()");
		}
		free(lpBuffer);
		return -1;
	} else if (nread == 0) {
		//Nothing read yet

		const int pollTimeout = (*env)->GetIntField(env, sps,
				spsw_pollReadTimeout);
		int poll_result = poll(&fds[0], 2, pollTimeout);

		if (poll_result == 0) {
			//Timeout
			throw_TimeoutIOException(env, 0);
			free(lpBuffer);
			return -1;
		} else if ((poll_result < 0)) {
			throw_InterruptedIOExceptionWithError(env, 0,
					"readBytes poll: Error during poll");
			free(lpBuffer);
			return -1;
		} else {
			if (fds[1].revents == POLLIN) {
				//we can read from close_event_fd => port is closing
				free(lpBuffer);
				return -1;
			} else if (fds[0].revents == POLLIN) {
				//Happy path just check if its the right event...
				nread = read(fds[0].fd, lpBuffer, len);
				if (nread < 0) {
					if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
						free(lpBuffer);
						return -1;
					} else if (nread == 0) {
						throw_TimeoutIOException(env, 0); //Is this right???
						free(lpBuffer);
						return -1;
					} else {
						throw_InterruptedIOExceptionWithError(env, 0,
								"readBytes read error: Should never happen");
						free(lpBuffer);
						return -1;
					}
				}
			} else {
				throw_InterruptedIOExceptionWithError(env, 0,
						"readBytes poll: received poll event");
				free(lpBuffer);
				return -1;
			}
		}
	}

	int overallRead = nread;

	const int interByteTimeout = (*env)->GetIntField(env, sps,
			spsw_interByteReadTimeout);

	//Loop over poll and read to aquire as much bytes as possible either
	// a poll timeout, a full read buffer or an error
	// breaks the loop
	while (overallRead < len) {

		int poll_result = poll(&fds[0], 2, interByteTimeout);

		if (poll_result == 0) {
			//This is the interbyte timeout - We are done
			(*env)->SetByteArrayRegion(env, bytes, off, overallRead, lpBuffer);
			free(lpBuffer);
			return overallRead;
		} else if ((poll_result < 0)) {
			throw_InterruptedIOExceptionWithError(env, 0,
					"readBytes poll: Error during poll");
			free(lpBuffer);
			return -1;
		} else {
			if (fds[1].revents == POLLIN) {
				//we can read from close_event_fd => port is closing
				free(lpBuffer);
				return -1;
			} else if (fds[0].revents == POLLIN) {
				//Happy path
			} else {
				throw_InterruptedIOExceptionWithError(env, 0,
						"readBytes poll: received poll event");
				free(lpBuffer);
				return -1;
			}
		}

		//OK No timeout and no error, we should read at least one byte without blocking.

		nread = read(fds[0].fd, &lpBuffer[overallRead], len - overallRead);
		if (nread > 0) {
			overallRead += nread;
		} else {
			if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
				free(lpBuffer);
				return -1;
			} else if (nread == 0) {
				throw_InterruptedIOExceptionWithError(env, 0,
						"readBytes: nothing to read after successful polling: Should never happen");
			} else {
				throw_InterruptedIOExceptionWithError(env, 0,
						"readBytes read error: Should never happen");
				free(lpBuffer);
				return -1;
			}
		}
	}
	//We reached this, because the read buffer is full.
	(*env)->SetByteArrayRegion(env, bytes, off, overallRead, lpBuffer);
	free(lpBuffer);
	return overallRead;
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    readSingle
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_readSingle(
		JNIEnv *env, jobject sps) {

	//port is nonblocking and no hardware interbyte timeout, so we try to reed one byte.
	struct pollfd fds[2];
	fds[0].fd = (*env)->GetIntField(env, sps, spsw_fd);
	fds[0].events = POLLIN;
	fds[1].fd = (*env)->GetIntField(env, sps, spsw_closeEventFd);
	fds[1].events = POLLIN;

	jbyte lpBuffer;
	int nread = read(fds[0].fd, &lpBuffer, 1);
	if (nread == 1) {
		return lpBuffer & 0xFF;
	} else if (nread < 0) {
		if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
			//Filehandle not valid -> closed.
		} else {
			throw_InterruptedIOExceptionWithError(env, 0,
					"readSingle: read error during first invocation of read()");
		}
		return -1;
	}

	//Nothing was read so use poll to wait...
	const int pollTimeout = (*env)->GetIntField(env, sps, spsw_pollReadTimeout);

	int poll_result = poll(&fds[0], 2, pollTimeout);

	if (poll_result == 0) {
		//Timeout
		throw_TimeoutIOException(env, 0);
		return -1;
	} else if ((poll_result < 0)) {
		throw_InterruptedIOExceptionWithError(env, 0,
				"readSingle poll: Error during poll");
		return -1;
	} else {
		if (fds[1].revents == POLLIN) {
			//we can read from close_event_fd => port is closing
			return -1;
		} else if (fds[0].revents == POLLIN) {
			//Happy path just check if its the right event...
		} else {
			throw_InterruptedIOExceptionWithError(env, 0,
					"readSingle poll: received poll event");
			return -1;
		}
	}

	//OK polling succeeds we should be able to read the byte.
	nread = read(fds[0].fd, &lpBuffer, 1);
	if (nread == 1) {
		return lpBuffer & 0xFF;
	}

	if ((*env)->GetIntField(env, sps, spsw_fd) == INVALID_FD) {
		//Closed no-op
	} else {
		throw_InterruptedIOExceptionWithError(env, 0,
				"readSingle read nothing read and no timeout => Should never happen");
	}
	return -1;
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    sendBreak
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_sendBreak(
		JNIEnv *env, jobject sps, jint duration) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);
	if (tcsendbreak(fd, duration) != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't sendBreak");
	}
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    setBreak
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_setBreak
(JNIEnv *env, jobject sps, jboolean enabled) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);
	int arg;
	if (enabled == JNI_TRUE) {
		arg = TIOCSBRK;
	} else {
		arg = TIOCCBRK;
	}
	if (ioctl(fd, arg) != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't set Break");
	}
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    setDTR
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_setDTR
(JNIEnv *env, jobject sps, jboolean enabled) {
	setLineStatus(env, sps, enabled, TIOCM_DTR);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    setParameters
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_setParameters(
		JNIEnv *env, jobject sps, jint parameterBitSet) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);

	struct termios settings;
	if (tcgetattr(fd, &settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "setParameters tcgetattr");
		return;
	}

	setParams(env, sps, &settings, parameterBitSet);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    setRTS
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_setRTS
(JNIEnv *env, jobject sps, jboolean enabled) {
	setLineStatus(env, sps, enabled, TIOCM_RTS);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    setXOFFChar
 * Signature: (C)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_setXOFFChar
(JNIEnv *env, jobject sps, jchar c) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);

	struct termios settings;
	if (tcgetattr(fd, &settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "setXOFFChar tcgetattr");
		return;
	}
	settings.c_cc[VSTOP] = c;

	if (tcsetattr(fd, TCSANOW, &settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "setXOFFChar tcsetattr");
	}

}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    setXONChar
 * Signature: (C)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_setXONChar
(JNIEnv *env, jobject sps, jchar c) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);

	struct termios settings;
	if (tcgetattr(fd, &settings) != 0) {
		throw_ClosedOrNativeException(env, sps, "setXONChar tcgetattr");
		return;
	}
	settings.c_cc[VSTART] = c;

	if (tcsetattr(fd, TCSANOW, &settings) != 0) {
		//TODO EBADF == errno
		throw_ClosedOrNativeException(env, sps, "setXONChar tcsetattr");
	}

}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    writeBytes
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_writeBytes
(JNIEnv *env, jobject sps, jbyteArray bytes, jint off, jint len) {

	const int fd = (*env)->GetIntField(env, sps, spsw_fd);
	jbyte* buf = (jbyte*) malloc(len);
	(*env)->GetByteArrayRegion(env, bytes, off, len, buf);
	if ((*env)->ExceptionOccurred(env)) {
		free(buf);
		return;
	}
	const int pollTimeout = (*env)->GetIntField(env, sps, spsw_pollWriteTimeout);

//See javaTimeNanos() in file src/os/linux/vm/os_linux.cpp of hotspot sources
	struct timespec endTime;
	clock_gettime(CLOCK_MONOTONIC, &endTime);

	int written = write(fd, buf, len);

	if (written == len) {
		//all was written
		free(buf);
		return;
	}

	if (written < 0) {
		if (EAGAIN == errno) {
			written = 0;
		} else {
			throw_IO_ClosedOrInterruptedException(env, sps, 0, "unknown port error  writeBytes");
			free(buf);
			return;
		}
	}

	struct pollfd fds[2];
	fds[0].fd = fd;
	fds[0].events = POLLOUT;
	fds[1].fd = (*env)->GetIntField(env, sps, spsw_closeEventFd);
	fds[1].events = POLLIN;

	int offset = written;
	//calculate the endtime...
	if (pollTimeout > 0) {
		endTime.tv_sec += pollTimeout / 1000;	//full seconds
		endTime.tv_nsec += (pollTimeout % 1000) * 1000000;// reminder goes to nanos
		if (endTime.tv_nsec > 1000000000) {
			//Overflow occured
			endTime.tv_sec += 1;
			endTime.tv_nsec -= 1000000000;
		}
	}

	do {
		struct timespec currentTime;
		clock_gettime(CLOCK_MONOTONIC, &currentTime);

		jlong remainingTimeOut = (pollTimeout == -1) ? -1 : (endTime.tv_sec - currentTime.tv_sec) * 1000L + (endTime.tv_nsec - currentTime.tv_nsec) / 1000000L;

		int poll_result = poll(&fds[0], 2, remainingTimeOut);

		if (poll_result == 0) {
			//Timeout occured
			throw_TimeoutIOException(env, written);
			free(buf);
			return;
		} else if ((poll_result < 0)) {
			throw_InterruptedIOExceptionWithError(env, offset, "poll timeout with error writeBytes");
			free(buf);
			return;
		} else {
			if (fds[1].revents == POLLIN) {
				//we can read from close_event_fd => port is closing
				throw_IO_ClosedException(env, offset);
				free(buf);
				return;
			} else if (fds[0].revents == POLLOUT) {
				//Happy path all is right...
			} else {
				throw_InterruptedIOExceptionWithError(env, offset, "poll returned with poll event writeBytes");
				free(buf);
				return;
			}
		}

		written = write(fd, &buf[offset], len - offset);

		if (written < 0) {
			throw_IO_ClosedOrInterruptedException(env, sps, offset, "writeBytes after poll no bytes written");
			free(buf);
			return;
		}

		offset += written;

	}while (offset < len);

	free(buf);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
 * Method:    writeSingle
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_writeSingle
(JNIEnv *env, jobject sps, jint b) {
	const int fd = (*env)->GetIntField(env, sps, spsw_fd);
	int written = write(fd, &b, 1);

	if (written == 1) {
		return;
	}
	if (written == 0) {
		//No-op do poll
	} else if (written < 0) {
		if (EAGAIN == errno) {
			//No-op do poll
			written = 0;
		} else {
			throw_IO_ClosedOrInterruptedException(env, sps, 0, "unknown error writeSingle");
			return;
		}
	}

	const int pollTimeout = (*env)->GetIntField(env, sps, spsw_pollWriteTimeout);
	struct pollfd fds[2];
	fds[0].fd = fd;
	fds[0].events = POLLOUT;
	fds[1].fd = (*env)->GetIntField(env, sps, spsw_closeEventFd);
	fds[1].events = POLLIN;

	int poll_result = poll(&fds[0], 2, pollTimeout);

	if (poll_result == 0) {
		//Timeout
		throw_TimeoutIOException(env, written);
		return;
	} else if (poll_result < 0) {
		throw_InterruptedIOExceptionWithError(env, written, "writeSingle poll: Error during poll");
		return;
		if (fds[1].revents == POLLIN) {
			//we can read from close_event_fd => port is closing
			throw_IO_ClosedException(env, written);
			return;
		} else if (fds[0].revents == POLLOUT) {
			//Happy path all is right...
		} else {
			throw_InterruptedIOExceptionWithError(env, written, "writeSingle error during poll");
			return;
		}
	}

	written = write(fd, &b, 1);
	if (written == 0) {
		throw_TimeoutIOException(env, written);
	} else if (written < 0) {
		throw_IO_ClosedOrInterruptedException(env, sps, 0, "writeSingle too few bytes written");
	}
}

/*
 * Class:     de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket
 * Method:    drainOutputBuffer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket_drainOutputBuffer
(JNIEnv *env, jobject sps) {

	const int pollTimeout = (*env)->GetIntField(env, sps, spsw_pollWriteTimeout);
	struct pollfd fds[2];
	fds[0].fd = (*env)->GetIntField(env, sps, spsw_fd);
	fds[0].events = POLLOUT;
	fds[1].fd = (*env)->GetIntField(env, sps, spsw_closeEventFd);
	fds[1].events = POLLIN;

	int poll_result = poll(&fds[0], 2, pollTimeout);

	if (poll_result == 0) {
		//Timeout
		throw_TimeoutIOException(env, 0);
		return;
	} else if ((poll_result < 0)) {
		throw_IOException_NativeError(env, "drainOutputBuffer poll: Error during poll");
		return;
	} else {
		if (fds[1].revents == POLLIN) {
			//we can read from close_event_ds => port is closing
			throw_IOException_Closed(env);
			return;
		} else if (fds[0].revents == POLLOUT) {
			//Happy path all is right... no-op
		} else {
			// closed?
			throw_ClosedOrNativeException(env, sps, "drainOutputBuffer poll => : received unexpected event and port not closed");
			return;
		}
	}

	int result = tcdrain(fds[0].fd);
	if (result != 0) {
		throw_ClosedOrNativeException(env, sps, "Can't drain the output buffer");
	}
}

/*
 * Class:     de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket
 * Method:    isDTR
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket_isDTR(
		JNIEnv *env, jobject sps) {
	return getLineStatus(env, sps, TIOCM_DTR);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket
 * Method:    isRTS
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket_isRTS(
		JNIEnv *env, jobject sps) {
	return getLineStatus(env, sps, TIOCM_RTS);
}

/*
 * Class:     de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket
 * Method:    sendXOFF
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket_sendXOFF
(JNIEnv *env, jobject sps) {
	throw_Illegal_Argument_Exception(env, "setXOFF not implemented yet");
}

/*
 * Class:     de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket
 * Method:    sendXON
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericTermiosSerialPortSocket_sendXON
(JNIEnv *env, jobject sps) {
//TODO How ??? tcflow ?
	throw_Illegal_Argument_Exception(env, "setXON not implemented yet");
}