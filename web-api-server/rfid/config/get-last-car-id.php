<?php
    include('database-config.php'); 
    $sql = "SELECT  * FROM car ORDER BY car.id DESC LIMIT 1";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo ($row['id']);
        return;
    }
    echo("C00001");
?>