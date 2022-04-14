module com.example.realui {
    requires javafx.controls;
    requires javafx.fxml;


    opens com.example.realui to javafx.fxml;
    exports com.example.realui;
}