$(document).ready(function() {
    $("#__IPT__").on("keyup", function() {
        var value = $(this).val().toLowerCase();
        $("#__TBL__ tr").filter(function() {
            $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1)
        });
    });
});
