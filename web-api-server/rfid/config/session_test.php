<?php
session_start();

//create session;
$_SESSION['isLogin1'] = "FUEYMC";


// $_SESSION['isLogin23'] = null;

//remove session
unset($_SESSION['isLogin23']);

//echo seesion;
// echo $_SESSION['isLogin'];


// session_destroy();

//print all session;
echo print_r($_SESSION);

?>