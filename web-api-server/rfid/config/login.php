<?php
    include('database-config.php');

    $username = isset($_REQUEST['username']) ? $_REQUEST['username'] : "" ;
    $password = isset($_REQUEST['password']) ? $_REQUEST['password'] : "" ;

    if($username == '' || $password == ''){
        echo "<script>alert('กรุณากรอกข้อมูลให้ครบ'); window.location = '../index.php'; </script>";
        return;
    }

    $sql = "SELECT * FROM `admin` WHERE (username = '".$username ."' AND password = '" .$password ."');";
    $result = $conn->query($sql);
    if($result->num_rows == 0){
        echo "<script>alert('ชื่อผู้ใช้งานหรือรหัสผ่านไม่ถูกต้อง'); window.location = '../index.php'; </script>";
        return;
    }
    
    //Set start session
    session_start();
    $_SESSION["admin"] = $username;
    $_SESSION['time'] = time();
    echo "<script> window.location = '../setting.php'; </script>";
?>