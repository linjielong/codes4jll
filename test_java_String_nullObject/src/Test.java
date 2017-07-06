/*
 * Copyright(c) 2016-2100.  jielong.lin.  All rights reserved.
 */
/*
 * FileName:      Test.java
 * Author:        jielong.lin
 * Email:         493164984@qq.com
 * DateTime:      2017-07-06 15:00:58
 * ModifiedTime:  2017-07-06 15:00:58
 */


package src;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class Test
{
    public static void main(String[] args) throws Exception
    {
        String xxx=null;
        //xxx=xxx+"/who";
        if (xxx.endsWith("who")) {
            System.out.println("Hello World Right "+xxx+"/who");
        }
        xxx=xxx+"/who";
        System.out.println("Hello World "+xxx+"/who");
    }
}

