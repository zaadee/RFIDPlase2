$( document ).ready(function() {
    //alert( "ready!" );
    startRefreshLoop();
});

function startRefreshLoop() {
    //console.log("ควาย");

    $.ajax({
        url: ("../rfid/api/index.php?action=is_new_data"),
        method: "GET"
    }).then(function (response) {
        console.log(response);
        //console.log(json["status"]);
        if(response){
            //console.log(json.message);
            window.location.reload(1);
        }
    });

    setTimeout(function () {        //  call a 2s setTimeout when the loop is called

        
        startRefreshLoop();         //  ..  again which will trigger another                          
     }, 2000)//  ..  setTimeout()
}