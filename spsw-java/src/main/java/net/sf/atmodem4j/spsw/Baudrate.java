package net.sf.atmodem4j.spsw;

/*
 * #%L
 * SPSW Java
 * %%
 * Copyright (C) 2009 - 2014 atmodem4j
 * %%
 * atmodem4j - A serial port socket wrapper- http://atmodem4j.sourceforge.net/
 * Copyright (C) 2009-2014, atmodem4j.sf.net, and individual contributors as indicated
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

/**
 *
 * @author aploese
 */
public enum Baudrate {

            B0(0),
            B50(50),
            B75(75),
            B110(110),
            B134(134),
            B150(150),
            B200(200),
            B300(300),
            B600(600),
            B1200(1200),
            B1800(1800),
            B2400(2400),
            B4800(4800),
            B9600(9600),
            B19200(19200),
            B38400(38400),
            B57600(57600),
            B115200(115200),
            B230400(230400),
            B460800(460800),
            B500000(500000),
            B576000(576000),
            B921600(921600),
            B1000000(1000000),
            B1152000(1152000),
            B1500000(1500000),
            B2000000(2000000),
            B2500000(2500000),
            B3000000(3000000),
            B3500000(3500000),
            B4000000(4000000);

    public static Baudrate fromValue(int baudrate) {
        return fromNative(baudrate);
    }
    
    static Baudrate fromNative(int baudrate) {
        switch (baudrate) {
            case 0: return B0;
            case 50: return B50;
            case 75: return B75;
            case 110: return B110;
            case 134: return B134;
            case 150: return B150;
            case 200: return B200;
            case 300: return B300;
            case 600: return B600;
            case 1200: return B1200;
            case 1800: return B1800;
            case 2400: return B2400;
            case 4800: return B4800;
            case 9600: return B9600;
            case 19200: return B19200;
            case 38400: return B38400;
            case 57600: return B57600;
            case 115200: return B115200;
            case 230400: return B230400;
            case 460800: return B460800;
            case 500000: return B500000;
            case 576000: return B576000;
            case 921600: return B921600;
            case 1000000: return B1000000;
            case 1152000: return B1152000;
            case 1500000: return B1500000;
            case 2000000: return B2000000;
            case 2500000: return B2500000;
            case 3000000: return B3000000;
            case 3500000: return B3500000;
            case 4000000: return B4000000;
                default: throw new RuntimeException("Non standard baudrate " + baudrate);
        }
    }

public final int value;

    private Baudrate(int baudrate) {
        this.value = baudrate;
    }
    
}
