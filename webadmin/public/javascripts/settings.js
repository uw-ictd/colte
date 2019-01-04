$(document).ready(function() {
    $("#dns").change(function(event) {
        if (event.target.checked) {
            // TODO: Add warning, disable other services
        }
        
        $("#extra-fields").toggleClass("invisible");
    });
});