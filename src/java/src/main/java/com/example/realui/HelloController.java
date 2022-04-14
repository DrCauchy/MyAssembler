package com.example.realui;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.layout.AnchorPane;
import javafx.stage.FileChooser;
import javafx.stage.DirectoryChooser;
import javafx.stage.Stage;

import java.io.*;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.*;

public class HelloController {
    @FXML
    private TextField projectName;

    @FXML
    private TextField save;

    @FXML
    private TabPane tabPane;

    @FXML
    private TextField forwardReads;

    @FXML
    private TextField reverseReads;

    @FXML
    private TextField seed;

    @FXML
    private TextField kmer;

    @FXML
    private TextField votingRate;

    @FXML
    private TextField minContig;

    @FXML
    private TextField maxContig;

    @FXML
    private TextField standardOverlap;

    @FXML
    private TextField _t;

    @FXML
    private TextField _m;

    @FXML
    private TextField _e;

    @FXML
    private TextField _v;

    @FXML
    private TextField _x;

    @FXML
    private CheckBox log;

    @FXML
    private Button run;

    @FXML
    private Button frbtn;

    @FXML
    private Button rrbtn;

    @FXML
    private Button sbtn;

    @FXML
    private Button svbtn;

    @FXML
    private AnchorPane filePane;
    @FXML
    private ListView<String> list;

    private String[] exec;
    private static final ObservableList<String> ob = FXCollections.observableArrayList();

    static boolean findPath(String filename,File dir, String[] exePath)
    {

        File[]files=dir.listFiles();
        assert files != null;
        for(File file:files)
        {
            if(file.isDirectory())                      //file是目录时，则重新调用function函数;
            {
                if(findPath(filename,file.getAbsoluteFile(), exePath)) return true;
            }
            if(file.isFile() && filename.equals(file.getName()))  //file时文件且文件名相同时，输出;
            {
                exePath[0] = file.getAbsolutePath();
                System.out.println("要查找的文件路径为："+file.getAbsolutePath());
                return true;
            }
        }
        System.out.print(dir.getName()+"文件夹下没有找到此文件");
        return false;
    }

    public void Run(ActionEvent event) {

        File file1 = new File(".");
        System.out.println(". 代表的绝对路径为" + file1.getAbsolutePath());
        String OS = System.getProperty("os.name").toLowerCase();
//        System.out.println(OS);
        list.setItems(ob);
        Map<String, String> params = new HashMap<String, String>();

        //必要参数
        params.put("-r", reverseReads.getText());
        params.put("-f", forwardReads.getText());
        params.put("-s", seed.getText());

        params.put("-z", save.getText() + "\\" + projectName.getText());

        //可选参数
        if (!Objects.equals(kmer.getText(), "")) {
            params.put("-k", kmer.getText());
        }else{
            params.put("-k", "39");
        }
        if (!Objects.equals(maxContig.getText(), "")) {
            params.put("-q", maxContig.getText());
        }else{
            params.put("-q", "0");
        }
        if (!Objects.equals(votingRate.getText(), "")) {
            params.put("-c", votingRate.getText());
        }else{
            params.put("-c", "0.6");
        }
        if (!Objects.equals(standardOverlap.getText(), "")) {
            params.put("-n", standardOverlap.getText());
        }else{
            params.put("-n", "18");
        }
        if (!Objects.equals(minContig.getText(), "")) {
            params.put("-o", minContig.getText());
        }else{
            params.put("-o", "0");
        }
        if (!Objects.equals(_t.getText(), "")) {
            params.put("-t", _t.getText());
        }else{
            params.put("-t", "5");
        }
        if (!Objects.equals(_m.getText(), "")) {
            params.put("-m", _m.getText());
        }else{
            params.put("-m", "16");
        }
        if (!Objects.equals(_e.getText(), "")) {
            params.put("-e", _e.getText());
        }else{
            params.put("-e", "0");
        }
        if (!Objects.equals(_v.getText(), "")) {
            params.put("-v", _v.getText());
        }else{
            params.put("-v", "3");
        }

        if (!Objects.equals(_x.getText(), "")) {
            params.put("-x", _x.getText());
        }else{
            params.put("-x", "0");
        }
        if (log.isSelected()) {
            params.put("-l", save.getText() + "log");
            exec = new String[31];
        }else{
            exec = new String[29];
        }
        // 检查kmer
        int Kmer = Integer.parseInt(params.get("-k"));
        if (Kmer % 2 == 0) {
            System.out.println("ERROR IN KMER, ODD NUMBER ONLY!");
//            prompt.setText("ERROR IN KMER, ODD NUMBER ONLY!");
            return;
        }
        //读取可执行程序路径

        File dir = new File("./bin");
        String exeName = OS.contains("windows")  ? "MyAssembler.exe" : "MyAssembler";
        if(!findPath(exeName, dir, exec)){
            ob.add("EXE_PATH ERROR!");
            return;
        }

        int i = 1;
        for (Map.Entry<String, String> entry : params.entrySet()) {
            System.out.println("Key = " + entry.getKey() + ", Value = " + entry.getValue());
            exec[i++] = entry.getKey();
            exec[i++] = entry.getValue();
        }
        System.out.println(Arrays.toString(exec));
//        prompt.setText(Arrays.toString(exec));

        final Runtime runtime = Runtime.getRuntime();
        Process process = null;
        try {
            process = runtime.exec(exec);
            //获取进程的标准输入流
            final InputStream is1 = process.getInputStream();
            //获取进城的错误流
            final InputStream is2 = process.getErrorStream();
            //启动两个线程，一个线程负责读标准输出流，另一个负责读标准错误流
            final String[] line1 = {null};
            new Thread() {
                public void run() {
                    BufferedReader br1 = new BufferedReader(new InputStreamReader(is1));
                    try {

                        while ((line1[0] = br1.readLine()) != null) {
//                            ob.add(line1[0]);
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    finally{
                        try {
                            is1.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }.start();
//
            new Thread() {
                public void  run() {
                    BufferedReader br2 = new  BufferedReader(new  InputStreamReader(is2));
                    try {
                        String line2 = null ;
                        while ((line2 = br2.readLine()) !=  null ) {
                            ob.add(line2);

                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    finally{
                        try {
                            is2.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }.start();

            process.waitFor();
            process.destroy();
            ob.add("Finished, output files have been written to " + save.getText());

            //数据评价
            try{
                System.out.println(save.getText() + "\\" + projectName.getText() + ".stat");
                FileInputStream inputStream = new FileInputStream(save.getText() + "\\" + projectName.getText() + ".stat");
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
                String str = null;
                while((str = bufferedReader.readLine()) != null)
                {
                    ob.add(str);
                }

                //close
                inputStream.close();
                bufferedReader.close();
            }catch (Exception e){
                System.out.println("err");
            }



        } catch (final Exception e) {
            System.out.println("Error exec!");
        }

    }


    public String chooseFile() {// 限制fasta文件
        Stage stage = (Stage) filePane.getScene().getWindow();
        System.out.println("choosefile");
        FileChooser fc = new FileChooser();
        fc.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("*.FASTA", "*.fasta", "*.fq"));

        File file = fc.showOpenDialog(stage);
        String path = file.getAbsolutePath();

        return path;
    }

    public String chooseDirectory() {// 限制fasta文件
        Stage stage = (Stage) filePane.getScene().getWindow();
        // System.out.println("choosefile");
        DirectoryChooser dc = new DirectoryChooser();

        File file = dc.showDialog(stage);
        String path = file.getAbsolutePath();

        return path;
    }

    public void fc1(ActionEvent event) {
        String a = chooseFile();
        forwardReads.setText(a);
    }

    public void fc2(ActionEvent event) {
        String a = chooseFile();
        reverseReads.setText(a);
    }

    public void fc3(ActionEvent event) {
        String a = chooseFile();
        seed.setText(a);
    }

    public void dc(ActionEvent event) {
        String a = chooseDirectory();
        save.setText(a);
    }

    public int check(TextField x) { // 检查是否可以转化为整数
        String str = x.getText();
        try {
            int X = Integer.parseInt(str);
            return X;
        } catch (NumberFormatException e) {
            System.out.println("Unable to convert input string :" + str + " to int");
            return -999;
        }
    }

}