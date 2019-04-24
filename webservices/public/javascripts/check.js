(function() {
    'use strict';

    window.onload = function() {
        const updateUrl = "/home/updateStatus";
        const checkUrl = "/home/checkStatus";
        var data = {
            checked: false,
            service: "",
        };
        $(".checkbox").each(function() {
            var current = this;
            data.checked = current.checked;
            data.service = current.id;
            $(current).prop('disabled', true);
            $.post(checkUrl, data) 
                .done(function(data, status, response){
                    alert("Response: " + JSON.stringify(response));
                    var response = response.responseText;
                    if (response == "enabled") {
                        $(current).prop('checked', true);
                    } else if (response == "disabled") {
                        $(current).prop('checked', false);
                    } else if (response == "not installed"){
                        alert("Service Misconfigured or Not Installed")
                        $(current).parent().hide();
                        $(current).parent().next().removeAttr('hidden');
                    }
                    $(current).prop('disabled', false);
                })
                .fail(function(xhr, status, error){
                    alert("Something Went Wrong! Status: " + xhr.responseText);
                    $(current).prop('checked', !current.checked);
                    $(current).prop('disabled', false);
                });
            changeDate(current);
        });

        $(".checkbox").change(function () {
            var current = this;
            data.checked = current.checked;
            data.service = current.id;
	        $(current).prop('disabled', true);
            $.post(updateUrl, data)
                .done(function(data, status){
                    $(current).prop('disabled', false);
                })
                .fail(function(xhr, status, error){
                    alert("Something Went Wrong! Status: " + xhr.responseText);
                    $(current).prop('checked', !current.checked);
                    $(current).prop('disabled', false);
                });
            changeDate(current);
        });

        function changeDate(current) {
            var dt = new Date();
            var time = fix(dt.getHours()) + ":" + fix(dt.getMinutes()) + ":" + fix(dt.getSeconds()) + " on " + 
                       fix(dt.getDay()) + "/" + fix(dt.getMonth() + 1) + "/" + dt.getFullYear();
            $(current).parent().parent().parent().find(".text-muted").html("Last updated at " + time);
            //alert(time);
        }

        function fix(number) {
            if (number < 10) {
                return "0" + number;
            } else {
                return "" + number;
            }
        }
    }
})();

function GetFormattedDate(date) {
    var month = format(date .getMonth() + 1);
    var day = format(date .getDate());
    var year = format(date .getFullYear());
    var hours = format(date .get)
    return month + "/" + day + "/" + year;
}
