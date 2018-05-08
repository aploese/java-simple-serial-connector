package de.ibapl.jnrheader;

import static java.lang.annotation.ElementType.TYPE;
import static java.lang.annotation.RetentionPolicy.RUNTIME;

import java.lang.annotation.Documented;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;

/**
 * Marks an Interface that support a function not available on all platforms or architectures.
 * 
 * @author aploese
 *
 */
@Documented
@Retention(RUNTIME)
@Target(TYPE)
public @interface NativeFunction {
}
