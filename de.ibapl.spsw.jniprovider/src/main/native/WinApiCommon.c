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
#include "spsw-jni.h"

#include "de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    static jboolean getCommModemStatus(JNIEnv *env, jobject sps, DWORD bitMask) {
        DWORD lpModemStat;

        HANDLE hFile = (HANDLE) (uintptr_t) (*env)->GetLongField(env, sps, spsw_fd);

        if (!GetCommModemStatus(hFile, &lpModemStat)) {
            throw_ClosedOrNativeException(env, sps, "Can't get GetCommModemStatus");
            return JNI_FALSE;
        }
        if ((lpModemStat & bitMask) == bitMask) {
            return JNI_TRUE;
        } else {
            return JNI_FALSE;
        }
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket_FdCleaner
     * Method:    closeFd
     * Signature: (J)V
     */
    JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket_00024FdCleaner_closeFd
    (__attribute__ ((unused)) JNIEnv *env, __attribute__ ((unused)) jclass clazz, jlong hFile) {
        HANDLE nativeHFile = (HANDLE) (uintptr_t) hFile;
        CancelIo(nativeHFile);
        CloseHandle(nativeHFile);
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    isCTS
     * Signature: ()Z
     */
    JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isCTS(
            JNIEnv *env, jobject sps) {
        return getCommModemStatus(env, sps, MS_CTS_ON);
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    isDSR
     * Signature: ()Z
     */
    JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isDSR(
            JNIEnv *env, jobject sps) {
        return getCommModemStatus(env, sps, MS_DSR_ON);
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    isDCD
     * Signature: ()Z
     */
    JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isDCD(
            JNIEnv *env, jobject sps) {
        return getCommModemStatus(env, sps, MS_RLSD_ON);
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    isIncommingRI
     * Signature: ()Z
     */
    JNIEXPORT jboolean JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_isRI(
            JNIEnv *env, jobject sps) {
        return getCommModemStatus(env, sps, MS_RING_ON);
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    close0
     * Signature: ()V
     */
    JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket_close0
    (JNIEnv *env, jobject sps) {

        HANDLE hFile = (HANDLE) (uintptr_t) (*env)->GetLongField(env, sps, spsw_fd);
        (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) INVALID_HANDLE_VALUE);
        // if only ReadIntervalTimeout is set and port is closed during pending read the read operation will hang forever...
        if (!CancelIo(hFile)) {
            if (GetLastError() != ERROR_NOT_FOUND) {
                throw_IOException_NativeError(env, "Can't cancel io for closing");
                (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) hFile);
                return;
            }
        }

        if (!CloseHandle(hFile)) {
            throw_IOException_NativeError(env, "Can't close port");
        }
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    getInBufferBytesCount
     * Signature: ()I
     */
    JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getInBufferBytesCount(
            JNIEnv *env, jobject sps) {
        DWORD lpErrors;
        COMSTAT comstat;

        HANDLE hFile = (HANDLE) (uintptr_t) (*env)->GetLongField(env, sps, spsw_fd);

        if (ClearCommError(hFile, &lpErrors, &comstat)) {
            return (jint) comstat.cbInQue;
        } else {
            throw_ClosedOrNativeException(env, sps,
                    "getInBufferBytesCount ClearCommError");
            return -1;
        }
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    getOutBufferBytesCount
     * Signature: ()I
     */
    JNIEXPORT jint JNICALL Java_de_ibapl_spsw_jniprovider_AbstractSerialPortSocket_getOutBufferBytesCount(
            JNIEnv *env, jobject sps) {
        DWORD lpErrors;
        COMSTAT comstat;

        HANDLE hFile = (HANDLE) (uintptr_t) (*env)->GetLongField(env, sps, spsw_fd);

        if (ClearCommError(hFile, &lpErrors, &comstat)) {
            return (jint) comstat.cbOutQue;
        } else {
            throw_ClosedOrNativeException(env, sps,
                    "getOutBufferBytesCount ClearCommError");
            return -1;
        }
    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_AbstractSerialPortSocket
     * Method:    open
     * Signature: (Ljava/lang/String;I)V
     */
    JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket_open0
    (JNIEnv *env, jobject sps, jstring portName, jint paramBitSet) {
        //Do not try to reopen port and therefore failing and overriding the file descriptor
        if ((HANDLE) (uintptr_t) (*env)->GetLongField(env, sps, spsw_fd) != INVALID_HANDLE_VALUE) {
            throw_IOException_Opend(env);
            return;
        }

        char prefix[] = "\\\\.\\";
        const char* port = (*env)->GetStringUTFChars(env, portName, JNI_FALSE);

        //string concat fix
        char portFullName[strlen(prefix) + strlen(port) + 1];
        strcpy(portFullName, prefix);
        strcat(portFullName, port);
        (*env)->ReleaseStringUTFChars(env, portName, port);

        HANDLE hFile = CreateFile(portFullName,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                NULL);

        if (hFile == INVALID_HANDLE_VALUE) {

            (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) INVALID_HANDLE_VALUE);

            switch (GetLastError()) {
                case ERROR_ACCESS_DENIED:
                    throw_IOException_withPortName(env, "Port is busy: \"%s\"", portName);
                    break;
                case ERROR_FILE_NOT_FOUND:
                    throw_IOException_withPortName(env, "Port not found: \"%s\"", portName);
                    break;
                default:
                    throw_IOException_NativeError(env, "Open");
            }
            return;
        }
        // The port is open, but maybe not configured ... setParam and getParam needs this to be set for their field access
        (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) hFile);

        DCB dcb;
        dcb.DCBlength = sizeof (DCB);
        if (!GetCommState(hFile, &dcb)) {
            CloseHandle(hFile);

            (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) INVALID_HANDLE_VALUE);

            throw_IOException_withPortName(env, "Not a serial port: \"%s\"", portName);
            return;
        }

        if (paramBitSet != SPSW_NO_PARAMS_TO_SET) {
            //set speed etc.
            if (setParams(env, sps, &dcb, paramBitSet)) {
                CloseHandle(hFile);
                (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) INVALID_HANDLE_VALUE);
                return;
            }
        }

        COMMTIMEOUTS lpCommTimeouts;
        if (!GetCommTimeouts(hFile, &lpCommTimeouts)) {
            CloseHandle(hFile);

            (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) INVALID_HANDLE_VALUE);

            throw_IOException_NativeError(env, "Open GetCommTimeouts");
            return;
        }

        lpCommTimeouts.ReadIntervalTimeout = 100;
        lpCommTimeouts.ReadTotalTimeoutConstant = 0;
        lpCommTimeouts.ReadTotalTimeoutMultiplier = 0;
        lpCommTimeouts.WriteTotalTimeoutConstant = 0;
        lpCommTimeouts.WriteTotalTimeoutMultiplier = 0;

        if (!SetCommTimeouts(hFile, &lpCommTimeouts)) {
            CloseHandle(hFile);

            (*env)->SetLongField(env, sps, spsw_fd, (intptr_t) INVALID_HANDLE_VALUE);

            throw_IOException_NativeError(env, "Open SetCommTimeouts");
            return;
        }

    }

    /*
     * Class:     de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket
     * Method:    getWindowsBasedPortNames
     * Signature: (Ljava/util/List;)V
     */
    JNIEXPORT void JNICALL Java_de_ibapl_spsw_jniprovider_GenericWinSerialPortSocket_getWindowsBasedPortNames(
            JNIEnv *env, __attribute__ ((unused)) jclass clazz, jobject list) {

        static jmethodID list_addID;
        if (list_addID == NULL) {
            jclass cls = (*env)->FindClass(env, "java/util/List");
            list_addID = (*env)->GetMethodID(env, cls, "add", "(Ljava/lang/Object;)Z");
            (*env)->DeleteLocalRef(env, cls);
            if (list_addID == NULL) {
                return;
            }
        }

        HKEY phkResult;
        LPCWSTR lpSubKey = L"HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &phkResult)
                == ERROR_SUCCESS) {
            boolean hasMoreElements = TRUE;
            WCHAR lpValueName[256];
            DWORD lpcchValueName;
            BYTE lpData[256];
            DWORD lpcbData;
            LSTATUS result;
            DWORD dwIndex = 0;
            DWORD lpType;
            do {
                lpcchValueName = 256;
                lpcbData = 256;
                result = RegEnumValueW(phkResult, dwIndex, (LPWSTR) &lpValueName,
                        &lpcchValueName, NULL, &lpType, (LPBYTE) & lpData, &lpcbData);
                if (result == ERROR_SUCCESS) {
                    if (lpType == REG_SZ) {
                        jvalue pName;
                        pName.l = (*env)->NewString(env, (uint16_t*) lpData, (int32_t)(lpcbData / sizeof (WCHAR) - 1));
                        if (pName.l == NULL) {
                            return;
                        }
                        (*env)->CallBooleanMethodA(env, list, list_addID, &pName);
                    } else {
                        //no-op just ignore 
                    }
                    dwIndex++;
                } else if (result == ERROR_NO_MORE_ITEMS) {
                    hasMoreElements = FALSE;
                }
            } while (hasMoreElements);
            CloseHandle(phkResult);
        }

    }



#ifdef __cplusplus
}
#endif
