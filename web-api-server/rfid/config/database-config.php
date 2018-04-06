<?php
    date_default_timezone_set("Asia/Bangkok");

    $servername_db = "localhost";
    $username_db = "root";
    $password_db = "1234qwer";
    $db_name = "rfid";

    // Create connection
    $conn = new mysqli($servername_db, $username_db, $password_db, $db_name);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 

    mysqli_set_charset($conn, "utf8");
?>