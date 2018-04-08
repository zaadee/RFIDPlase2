<?php
    if (!isset($_GET['carId'])) {
       return; 
    }

    include('database-config.php');
    $id = $_GET['carId'];
    $result = $conn->query("DELETE FROM car WHERE id = '" .$id ."';");
    
    if(!$result){
        echo false;
        return;
    }
    echo true;
?>