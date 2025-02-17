/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/UnitTests/JUnit4TestClass.java to edit this template
 */
package pkg2.weight;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author juanm
 */
public class MotxillaImplIT {
    
    private ElementMotxilla<String> a[];
    private Motxilla m;

    @Before
    public void setUp() {
        a = new ElementMotxilla[5];
        a[0] = new ElementMotxilla("A", 1.0, 2.0, 3.0);
        a[1] = new ElementMotxilla("B", 3.0, 1.0, 7.0);
        a[2] = new ElementMotxilla("C", 2.0, 1.0, 5.0);
        a[3] = new ElementMotxilla("D", 2.0, 1.0, 6.0);
        a[4] = new ElementMotxilla("E", 1.0, 2.0, 5.0);
        m = new MotxillaImpl();
    }

    /**
     * Test of recursiu method, of class MotxillaImpl.
     */
    @Test
    public void testRecursiu() {
        System.out.println("recursiu");
        test(m.recursiu(a, 3.0, 5.0));
    }

    /**
     * Test of iteratiu method, of class MotxillaImpl.
     */
    @Test
    public void testIteratiu() {
        System.out.println("iteratiu");
        test(m.iteratiu(a, 3.0, 5.0));
    }

    private void test(ElementMotxilla[] result) {
        assertEquals(3.0, ElementMotxilla.w1(result), 0.0);
        assertEquals(3.0, ElementMotxilla.w2(result), 0.0);
        assertEquals(11.0, ElementMotxilla.profit(result), 0.0);
        assertEquals(2, result.length);
        assertEquals("D", result[0].getElement());
        assertEquals("E", result[1].getElement());
    }
    
    /**
     * Test of recursiu method, of class MotxillaImpl.
     */
    @Test
    public void testRecursiu1() {
        System.out.println("recursiu1");
        ElementMotxilla[] result=m.recursiu(a, 4, 3);
       assertEquals(4.0, ElementMotxilla.w1(result), 0.0);
        assertEquals(3.0, ElementMotxilla.w2(result), 0.0);
        assertEquals(12.0, ElementMotxilla.profit(result), 0.0);
        assertEquals(2, result.length);
        assertEquals("B", result[0].getElement());
        assertEquals("E", result[1].getElement());
    }
    @Test
    public void testRecursiu2(){
         System.out.println("recursiu2");
        ElementMotxilla[] result=m.recursiu(a, 1, 2);
        assertEquals(1.0, ElementMotxilla.w1(result), 0.0);
        assertEquals(2.0, ElementMotxilla.w2(result), 0.0);
        assertEquals(5.0, ElementMotxilla.profit(result), 0.0);
        assertEquals(1, result.length);
        assertEquals("E", result[0].getElement());
        
        
    }
     @Test
    public void testIteratiu1() {
        System.out.println("iteratiu1");
        ElementMotxilla[] result=m.iteratiu(a, 4, 3);
       assertEquals(4.0, ElementMotxilla.w1(result), 0.0);
        assertEquals(3.0, ElementMotxilla.w2(result), 0.0);
        assertEquals(12.0, ElementMotxilla.profit(result), 0.0);
        assertEquals(2, result.length);
        assertEquals("B", result[0].getElement());
        assertEquals("E", result[1].getElement());
    }
    @Test
    public void testIteratiu2(){
         System.out.println("iteratiu2");
        ElementMotxilla[] result=m.iteratiu(a, 1, 2);
        assertEquals(1.0, ElementMotxilla.w1(result), 0.0);
        assertEquals(2.0, ElementMotxilla.w2(result), 0.0);
        assertEquals(5.0, ElementMotxilla.profit(result), 0.0);
        assertEquals(1, result.length);
        assertEquals("E", result[0].getElement());
        
        
    }
    
}
