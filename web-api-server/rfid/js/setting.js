$(document).on("click", ".ediCar", function () {
    var myBookId = $(this).data('id');
    $(".modal-body #bookId").val( myBookId );
});
