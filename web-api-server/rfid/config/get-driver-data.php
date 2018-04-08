<?php
    if (!isset($_GET['driverId'])) {
       return; 
    }

    include('database-config.php');
    $id = $_GET['driverId'];
    
    $result = $conn->query("SELECT * FROM driver WHERE id = '" .$id ."';");
    while($r = mysqli_fetch_assoc($result)) {
        echo json_encode($r, JSON_UNESCAPED_UNICODE);
    }
?>