<?php
    if (!isset($_GET['driverId'])) {
       return; 
    }

    include('database-config.php');
    $id = $_GET['driverId'];
    $result = $conn->query("DELETE FROM driver WHERE id = '" .$id ."';");
    
    if(!$result){
        echo false;
        return;
    }
    echo true;
?>