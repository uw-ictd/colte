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
            $.post(checkUrl, data, function(data, status, response){
                alert(JSON.stringify(response));
                if (status != "success") {
                    alert("Something Went Wrong!");
                    $(current).prop('checked', !current.checked);
                } else {
                    var response = response.responseText;
                    alert("RES " + response);
                    if (response == "enabled") {
                        $(current).prop('checked', true);
                    } else if (response == "disabled") {
                        $(current).prop('checked', false);
                    }
                }
            });
            changeDate(current);
            $(current).prop('disabled', false);
        });

        $(".checkbox").change(function () {
            var current = this;
            data.checked = current.checked;
            data.service = current.id;

            $.post(updateUrl, data, function(data, status){
                $(current).prop('disabled', true);
                alert("STAT " + status);
                if (status != "success") {
                    alert("Something Went Wrong!");
                    $(current).prop('checked', !current.checked);
                }
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
