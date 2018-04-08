<?php
    if (!isset($_GET['carId'])) {
       return; 
    }

    include('database-config.php');
    $id = $_GET['carId'];
    
    $sql = "SELECT * FROM car WHERE id = '" .$id ."';";
    $result = $conn->query($sql);
    $rows = array();

    while($r = mysqli_fetch_assoc($result)) {
        $rows[] = $r;
        echo json_encode($r,JSON_UNESCAPED_UNICODE);
    }
?>